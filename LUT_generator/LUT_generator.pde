//====== these parameters are available for the Haptic Servo ======
final int binLevels = 10;
final int binMin = 10;
final int binMax = 200;
final int freqLevels = 10;
final float freqMin = 10.0;
final float freqMax = 1000.0;

//====== LUT data ======
final int dataSize = 181;
final int maxDataIdx = dataSize-1;
float[] freqData = new float[dataSize];
float[] binData = new float[dataSize];


//====== GUI related constants ======

//====== sizes and positioning ======
final float guiCanvasHorizontalOffset = 20;
final float guiCanvasVerticalOffset = guiCanvasHorizontalOffset;
final float guiCanvasWidth = 1200;
final float guiCanvasHeight = 600;
final float guiCircleSize1 = 5;
final float guiCircleSize2 = 8;

//====== colors ======
final color guiStrokeColor = color(0);
final color guiCanvasColor = color(255);
final color guiBackgroundColor = color(102);
final color guiFrequencyColor = color(50, 50, 255);
final color guiBinColor = color(255, 50, 50);


final float xStep = guiCanvasWidth/(dataSize-1);





void settings() {
  size((int)(guiCanvasWidth+2*guiCanvasHorizontalOffset),
       (int)(guiCanvasHeight+2*guiCanvasVerticalOffset));
}


void setup() {
  ellipseMode(CENTER);
  
  for (int i = 0; i < dataSize; i++) {
    freqData[i] = freqMin;
    binData[i] = binMin;
  }
}


void draw() {
  background(guiBackgroundColor);
  strokeWeight(10);
  stroke(guiCanvasColor);
  fill(guiCanvasColor);
  rect(guiCanvasHorizontalOffset, guiCanvasVerticalOffset, guiCanvasWidth, guiCanvasHeight);
  strokeWeight(0);
  translate(guiCanvasHorizontalOffset, guiCanvasVerticalOffset);
  
  //====== choose the one of the functions to generate parametrization curves ====== 
  drawSawtoothPerStep();
  //drawNoSteps();
}


void drawSawtoothPerStep() {
  final float binsPerStep = dataSize / binLevels;
  final float binYstep = guiCanvasHeight / (binLevels-1);
  float binX = 0;
  float binY = guiCanvasHeight;
  float lastBinX = binX;
  float lastBinY = binY;
  
  final float freqYstep = guiCanvasHeight / (freqLevels-1);
  float freqX = 0;
  float freqY = guiCanvasHeight;
  float lastFreqX = freqX;
  float lastFreqY = freqY;
  
  for (int idx = 0; idx < dataSize; idx++) {
    // draw frequency graph (sawtooth function)
    freqX = idx * xStep;
    if (idx%(int(0.5+(binsPerStep))) == 0) {
      freqY = guiCanvasHeight;
    } else  {
      freqY -= (idx%(int(0.5+(binsPerStep/freqLevels))) != 0) ? 0 : freqYstep;
      //freqY -= freqYstep;
    }
    strokeWeight(1);
    stroke(guiFrequencyColor);
    line(lastFreqX, lastFreqY, freqX, freqY);
    lastFreqX = freqX;
    lastFreqY = freqY;
    strokeWeight(0);
    fill(guiFrequencyColor);
    circle(freqX, freqY, guiCircleSize2);
    
    // draw bin graph (step function)
    binX = idx * xStep;
    binY -= (idx==0 || idx%(int(0.5+(binsPerStep))) != 0) ? 0 : binYstep; 
    if (binY < 0) { binY = 0; }
    strokeWeight(1);
    stroke(guiBinColor);
    line(lastBinX, lastBinY, binX, binY);
    lastBinX = binX;
    lastBinY = binY;
    strokeWeight(0);
    fill(guiBinColor);
    circle(binX, binY, guiCircleSize1);
  }
}


void drawNoSteps() {
  final float binYstep = guiCanvasHeight / dataSize;
  float binX = 0;
  float binY = guiCanvasHeight;
  float lastBinX = binX;
  float lastBinY = binY;
    
  for (int idx = 0; idx < dataSize; idx++) {
    // draw bin graph (step function)
    binX = idx * xStep;
    binY -= binYstep; 
    if (binY < 0) { binY = 0; }
    strokeWeight(1);
    stroke(guiBinColor);
    line(lastBinX, lastBinY, binX, binY);
    lastBinX = binX;
    lastBinY = binY;
    strokeWeight(0);
    fill(guiBinColor);
    circle(binX, binY, guiCircleSize1);
  }
}
