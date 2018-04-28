/**
 * SHA256 GPU Miner
 *   @author EtherDream
 *   @create 2017/02/20
 *   @update 2017/06/24
 */
const THREAD_COUNT = 2048;
const ITER = 256;


let gl, program;
let fshaderCode, vshaderCode;
let runFlag;

let shaBuf = new ArrayBuffer(16);
let shaBufU8 = new Uint8Array(shaBuf);
let shaBufU32 = new Uint32Array(shaBuf);


function log(s) {
    txtLog.value += s + '\n';
}

//
// utils
//
function formatUnit(v) {
    let unit = ['', 'K', 'M', 'G'];
    let i = 0;
    while (v >= 1024 && i < unit.length - 1) {
        v /= 1024;
        i++;
    }
    return Math.round(v * 100) / 100 + unit[i];
}

function bytesToHex(inBuf) {
    let inLen = inBuf.length;
    let outStr = '';

    for (let i = 0; i < inLen; i++) {
        let byte = inBuf[i] & 0xff;
        let hex = byte.toString(16);
        if (byte < 16) {
            hex = '0' + hex;
        }
        outStr += hex;
    }
    return outStr;
}


function hexToBytes(inStr) {
    let outLen = inStr.length / 2;
    let outBuf = new Uint8Array(outLen);

    for (let i = 0; i < outLen; i++) {
        let byte = +('0x' + inStr.substr(i * 2, 2));
        if (isNaN(byte)) {
            return null;
        }
        outBuf[i] = byte;
    }
    return outBuf;
}

function bswap(v) {
    let a = v >> 24 & 0xff;
    let b = v >> 16 & 0xff;
    let c = v >>  8 & 0xff;
    let d = v >>  0 & 0xff;
    return (d << 24 | c << 16 | b << 8 | a) >>> 0;
}


function loadShader() {
    log('loading shader files');

    let p1 = fetch('fshader.c')
        .then(res => res.text())
        .then(code => {
            fshaderCode = code;
        });

    let p2 = fetch('vshader.c')
        .then(res => res.text())
        .then(code => {
            vshaderCode = code;
        });

    Promise
        .all([p1, p2])
        .then(v => {
            log('ready');
        });
}

// firefox do not support line continuation
const REMOVE_LINES = /Firefox/.test(navigator.userAgent);

function initShader(code, type) {
    if (REMOVE_LINES) {
        code = code.replace(/\\\n/g, ' ');
    }

    let shader = gl.createShader(type);
    gl.shaderSource(shader, code);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        log('compile: ' + gl.getShaderInfoLog(shader));
        throw 'err';
    }
    gl.attachShader(program, shader);
}


function getAttribLoc(name) {
    let loc = gl.getAttribLocation(program, name);
    if (loc == -1) {
        throw 'err';
    }
    return loc;
}

function getUniformLoc(name) {
    let loc = gl.getUniformLocation(program, name);
    if (loc == null) {
        throw 'err';
    }
    return loc;
}


let progPos = 0;

let timerID;
let lastPos;
let lastTime;

function timerUpdate() {
    let count = progPos - lastPos;
    let now = Date.now();
    let speed = count / (now - lastTime) * 1000;

    labSpeed.innerHTML = formatUnit(speed);
    lastTime = now;
    lastPos = progPos;
}


let threadLoc,
    startLoc;

function start() {
    log(`mining... (${THREAD_COUNT} threads)`);

    progPos = 0;
    lastPos = 0;

    // draw line (-1, 0) to (1, 0)
    let posAtrLoc = getAttribLoc('v_posx');
    gl.enableVertexAttribArray(posAtrLoc);
    gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer());

    let vecPosXArr = new Float32Array([-1, 1]);
    gl.bufferData(gl.ARRAY_BUFFER, vecPosXArr, gl.STATIC_DRAW);
    gl.vertexAttribPointer(posAtrLoc, 1, gl.FLOAT, false, 0, 0);


    gl.clearColor(.0, .0, .0, .0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    startLoc = getUniformLoc('u_start');

    // threadLoc = getUniformLoc('u_thread');
    // gl.uniform1f(threadLoc, THREAD_COUNT);

    timerID = setInterval(timerUpdate, 1000);
    beginTime = lastTime = Date.now();
    setTimeout(advance, 0);
}


let beginTime;

let picBuf = new ArrayBuffer(THREAD_COUNT * 1 * 4);
let picU8 = new Uint8Array(picBuf);
let picU32 = new Uint32Array(picBuf);

function advance() {
    gl.uniform1ui(startLoc, progPos);
    gl.drawArrays(gl.LINES, 0, 2);

    // gl.finish();
    gl.readPixels(0, 0, THREAD_COUNT, 1, gl.RGBA, gl.UNSIGNED_BYTE, picU8);

    if (!runFlag) {
        return;
    }

    // console.log(picU8);
    // console.log(picU32);
    // for (let i = 0; i < picU32.length; i++) {
    //     console.log(i, bswap(picU32[i]));
    // }
    // return;

    let hashNum = ITER * THREAD_COUNT;
    progPos += hashNum;
    progBar.value = progPos;

    for (let i = 0; i < THREAD_COUNT; i++) {
        let rgba = picU32[i];
        if (rgba === 0) {
            log('render error!');
            return;
        }
        if (rgba > 0 && rgba !== 0xffffffff) {
            found(rgba);
            return;
        }
    }

    if (progPos >= 0xffffffff) {
        log('not found! refresh and try again');
        runFlag = false;
        return;
    }
    setTimeout(advance, 0);
}

function stopTimer() {
    clearInterval(timerID);
    timerUpdate();
}


function found(nonce) {
    stopTimer();
    runFlag = false;

    let t = Date.now() - beginTime;
    log('found!');
    log(`time: ${t}ms`);

    let count = bswap(nonce);
    let speed = formatUnit(count / t * 1000);
    log(`avg speed: ${speed} hash/s`)

    // verify result
    shaBufU32[3] = nonce;

    let hex = bytesToHex(shaBufU8);
    let s1 = hex.substr(0, 24);
    let s2 = hex.substr(24);
    txtA.value = s2;

    let hashHex = sha256(shaBufU8);

    log('VERIFY:');
    log(`  sha256(${s1} ${s2}) = ${hashHex}`);

    log('VERIFY YOUSELF:');

    let code = `  php -r "var_dump( hash('sha256', hex2bin('${s1}' . '${s2}')) );"`;
    log(code);
}

function refresh() {
    let buf = new Uint8Array(24);
    crypto.getRandomValues(buf);

    let hex = bytesToHex(buf);
    txtQ.value = hex.substr(0, 24);
    txtA.value = '';
    labSpeed.innerHTML = '0';
}


function compile() {
    log('compile shader...');

    let param = {
        INPUT_0: bswap(shaBufU32[0]) + 'u',
        INPUT_1: bswap(shaBufU32[1]) + 'u',
        INPUT_2: bswap(shaBufU32[2]) + 'u',
        THREAD: THREAD_COUNT + '.',
        ITER: ITER + '.',
    };


    if (program) {
        gl.deleteProgram(program);
    }
    program = gl.createProgram();

    // let fshader = fshaderCode.replace(/\$\{([^\}]+)\}/g, (m, $1) => {
    //     let val = param[$1];
    //     return val;
    // });

    let fshader = fshaderCode.replace(/\bU_(\w+)/g, (m, $1) => {
        let val = param[$1];
        if (val == null) {
            throw 'missing ' + $1;
        }
        return ' ' + val;
    });

    fshader = fshader
        .replace('//[', '/*')
        .replace('//]', '*/');

    // console.log(fshader)

    initShader(fshader, gl.FRAGMENT_SHADER);
    initShader(vshaderCode, gl.VERTEX_SHADER);

    gl.linkProgram(program);
    gl.useProgram(program);

    log('compiled');
}

btnMine.onclick = function() {
    if (runFlag) {
        return;
    }

    let inputU8 = hexToBytes(txtQ.value);
    if (!inputU8) {
        alert('invalid hex data');
        return;
    }
    if (inputU8.length != 12) {
        alert('must be 24 hex chars');
        return;
    }

    let inputU32 = new Uint32Array(inputU8.buffer);
    shaBufU32[0] = inputU32[0];
    shaBufU32[1] = inputU32[1];
    shaBufU32[2] = inputU32[2];

    compile();

    runFlag = true;
    start();
};

btnStop.onclick = function() {
    if (!runFlag) {
        return;
    }
    runFlag = false;
    stopTimer();
    log('stop');
};

btnRefresh.onclick = function() {
    if (runFlag) {
        return;
    }
    refresh();
    log('refreshed');
}


function main() {
    let canvas = document.createElement('canvas');
    canvas.height = 1;
    canvas.width = THREAD_COUNT;

    // for debug
    // document.body.appendChild(canvas);
    try {
        gl = canvas.getContext('webgl2');
    } catch (err) {
        console.warn(err);
    }

    if (!gl) {
        log('WebGL2.0 not support!');
        return;
    }

    // TODO
    // 尝试用 framebuffer，
    // 能不能让 GPU 执行 Shader 时不阻塞浏览器 UI 线程

    refresh();
    loadShader();
}
main();
