
$(document).ready(() => {

    let pnmDecoder = Module.cwrap('pnmDecode', 'null', ['number', 'number', 'number'])
    let pnmGetWidth = Module.cwrap('getWidth', 'number', ['number', 'number'])
    let pnmGetHeight = Module.cwrap('getHeight', 'number', ['number', 'number'])

    let canvas = document.getElementById("pnm-canvas")
    canvas.setAttribute("width", "300")
    canvas.setAttribute("height", "300")

    $('#pnm-canvas').on('dragover dragenter', (e) => {
        e.stopPropagation()
        e.preventDefault()
    }).on('drop', (e) => {
        e.stopPropagation()
        e.preventDefault()
        console.log(e.originalEvent.dataTransfer.files)
        const files = e.originalEvent.dataTransfer.files
        const file = files[0]
        const fileReader = new FileReader()
        fileReader.readAsArrayBuffer(file)
        fileReader.onload = (e) => {
            const pnmFile = new Int8Array(e.target.result)
            console.log(pnmFile)
            const filePtr = Module._malloc(file.size)
            Module.HEAP8.set(pnmFile, filePtr)
            let imgWidth = pnmGetWidth(filePtr, file.size)
            let imgHeight = pnmGetHeight(filePtr, file.size)
            let imgSize = imgWidth * imgHeight * 4;
            const rawImgPtr = Module._malloc(imgSize)
            pnmDecoder(filePtr, rawImgPtr, file.size)
            Module._free(filePtr)
            const pixels = new Uint8ClampedArray(Module.HEAP8.buffer, rawImgPtr, imgSize)
            Module._free(rawImgPtr)
            const img = new ImageData(pixels, imgWidth, imgHeight)
            console.log(img)
            let displayWidth = imgWidth
            let displayHeight = imgHeight
            canvas.setAttribute("width", displayWidth)
            canvas.setAttribute("height", displayHeight)
            canvas.getContext('2d').putImageData(img, 0, 0)
            document.getElementById("pnm-file-info").innerText = "width: " + imgWidth + " height: " + imgHeight
        }
    })
})
