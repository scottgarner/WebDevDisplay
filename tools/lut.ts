const panelRows = 1;
const panelColumns = 4;

const pixelRows = 10;
const pixelColumns = 72;

const logicalGrid: Number[][] = [];

for (let panelY = 0; panelY < panelRows; panelY++) {
  for (let panelX = 0; panelX < panelColumns; panelX++) {
    const panelIndex = panelColumns * panelY + panelX;

    for (let pixelY = 0; pixelY < pixelRows; pixelY++) {
      const pixelRow: Number[] = [];
      for (let pixelX = 0; pixelX < pixelColumns; pixelX++) {
        const panelIndex = panelColumns * panelY + panelX;
        const panelOffset = panelIndex * (pixelRows * pixelColumns);

        const index = pixelColumns * pixelY + pixelX + panelOffset;

        if (pixelY % 2 == 0) {
          pixelRow.push(index);
          ``;
        } else {
          pixelRow.unshift(index);
        }
      }
      const logicalRow = panelY * pixelRows + pixelY;
      if (logicalGrid[logicalRow] == undefined) logicalGrid[logicalRow] = [];
      logicalGrid[logicalRow].push(...pixelRow);
    }
  }
}

console.log(JSON.stringify(logicalGrid.flat()));

/*
const zigzagByRow = true;

for (let panelY = 0; panelY < panelRows; panelY++) {
  for (let panelX = 0; panelX < panelColumns; panelX++) {
    const pixelPrimaryLines = zigzagByRow ? pixelRows : pixelColumns;
    const pixelSecondaryLines = zigzagByRow ? pixelColumns : pixelRows;
    for (let primary = 0; primary < pixelPrimaryLines; primary++) {
      const pixelLine: Number[] = [];
      for (let secondary = 0; secondary < pixelSecondaryLines; secondary++) {
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
      logicalGrid[panelY * pixelPrimaryLines + primary].unshift(...pixelLine);
    }
  }
}

console.log(logicalGrid.flat);
if (zigzagByRow) {
  console.info(JSON.stringify(logicalGrid.flat()));
} else {
  const invertedLogicalGrid: Number[][] = logicalGrid[0].map((_, x) =>
    logicalGrid.map((row) => row[x])
  );
  console.info(JSON.stringify(invertedLogicalGrid.flat()));
}

*/
