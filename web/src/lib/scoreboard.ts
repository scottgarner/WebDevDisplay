interface ScoreboardOptions {
  saveBuffer?: boolean;
  scrollSpeed?: number;
}

export class Scoreboard {
  createBuffer(
    canvas: HTMLCanvasElement,
    options: ScoreboardOptions = {}
  ): Uint8Array {
    const header = new Uint16Array([
      0, // Currently not used.
      canvas.width, // Width.
      canvas.height, // Height.
      options.scrollSpeed || 0, // X-axis scroll speed.
      0, // Y-axis currently always zero.
      options.saveBuffer ? 1 : 0, // Save the buffer to the device?
    ]);

    const bufferSize = header.byteLength + canvas.width * canvas.height * 3;
    const buffer = new Uint8Array(bufferSize);
    buffer.set(new Uint8Array(header.buffer), 0);

    const context = canvas.getContext("2d");
    const imageData = context!.getImageData(0, 0, canvas.width, canvas.height);

    for (let i = 0; i < canvas.height; i++) {
      for (let j = 0; j < canvas.width; j++) {
        const index = (i * canvas.width + j) * 4;

        buffer[header.byteLength + (i * canvas.width + j) * 3] =
          imageData.data[index]; // Red
        buffer[header.byteLength + (i * canvas.width + j) * 3 + 1] =
          imageData.data[index + 1]; // Green
        buffer[header.byteLength + (i * canvas.width + j) * 3 + 2] =
          imageData.data[index + 2]; // Blue

        console.log(imageData.data[index]);
      }
    }

    return buffer;
  }
}
