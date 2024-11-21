const logicalGrid: Number[][] = [];

const panelRows = 1;
const panelColumns = 1;

const pixelRows = 8;
const pixelColumns = 8;

const zigzagByRow = false;

for (let panelY = 0; panelY < panelRows; panelY++) {
  for (let panelX = 0; panelX < panelColumns; panelX++) {
    const pixelPrimaryLines = zigzagByRow ? pixelRows : pixelColumns;
    const pixelSecondaryLines = zigzagByRow ? pixelColumns : pixelRows;
    for (let primary = 0; primary < pixelPrimaryLines; primary++) {
      const pixelLine: Number[] = [];
      for (let secondary = 0; secondary < pixelSecondaryLines; secondary++) {
        const pixelX = zigzagByRow ? secondary : primary;
        const pixelY = zigzagByRow ? primary : secondary;

        const panelIndex = panelColumns * panelY + panelX;
        const panelOffset = panelIndex * pixelRows * pixelColumns;
        const index = pixelSecondaryLines * primary + secondary + panelOffset;

        const logicalRowIndex = panelY * pixelPrimaryLines + primary;
        if (logicalGrid[logicalRowIndex] == undefined)
          logicalGrid[logicalRowIndex] = [];

        if (primary % 2 == 0) {
          pixelLine.push(index);
        } else {
          pixelLine.unshift(index);
        }
      }
      logicalGrid[panelY * pixelPrimaryLines + primary].push(...pixelLine);
    }
  }
}

if (zigzagByRow) {
  console.log(logicalGrid.flat());
} else {
  const invertedLogicalGrid: Number[][] = logicalGrid[0].map((_, x) =>
    logicalGrid.map((row) => row[x])
  );
  console.log(invertedLogicalGrid.flat());
}

/*
  0, 7, 8, 15, 16, 23, 24, 31,
  1, 6, 9, 14, 17, 22, 25, 30,
  2, 5, 10, 13, 18, 21, 26, 29,
  3, 4, 11, 12, 19, 20, 27, 28
*/
