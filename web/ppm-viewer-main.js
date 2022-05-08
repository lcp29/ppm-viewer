
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
            var renderer = document.createElement('canvas')
            renderer.width = imgWidth
            renderer.height = imgHeight
            renderer.getContext('2d').putImageData(img, 0, 0)

            let displayWidth = imgWidth
            let displayHeight = imgHeight
            if (displayWidth <= 200 && displayWidth >= displayHeight) {
                displayWidth = 200 / displayHeight * displayWidth
                displayHeight = 200
            } else if (displayHeight <= 200 && displayWidth < displayHeight) {
                displayHeight = 200 / displayWidth * displayHeight
                displayWidth = 200
            }

            if (displayWidth >= 1000 && displayWidth <= displayHeight) {
                displayWidth = 1000 / displayHeight * displayWidth
                displayHeight = 1000
            } else if (displayHeight >= 1000 && displayWidth >= displayHeight) {
                displayHeight = 1000 / displayWidth * displayHeight
                displayWidth = 1000
            }
            canvas.setAttribute("width", displayWidth)
            canvas.setAttribute("height", displayHeight)
            canvas.getContext('2d').drawImage(renderer, 0, 0, displayWidth, displayHeight)
            document.getElementById("pnm-file-info").innerText = "width: " + imgWidth + " height: " + imgHeight
        }
    })
})
