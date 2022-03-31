enum Waveform {
  Triangle,
  TriangleInverse,
  Sawtooth,
  SawtoothInverse
};

enum Function {
  Continuous,
  Step,
  StepsPerStep
}


/********************************************************************
                      USER DEFINED PARAMETERS
********************************************************************/

//====== function configuration ======
final Waveform waveForm = Waveform.Triangle;
final Function function = Function.Continuous;


//====== these parameters are available for the Haptic Servo ======
// define ranges used in all functions to clamp the output
final int binMin = 10;
final int binMax = 100;
final float freqMin = 10.0;
final float freqMax = 300.0;
// this variables are only used in StepsPerStep
final int binLevels = 12;
final int freqLevels = 8;
// this variable is only used in Continuous
final int periodeSteps = 6;
// this variable is only used in Step
final int steps = 10;



/********************************************************************
                     SYSTEM DEFINED PARAMETERS
********************************************************************/

//====== LUT data ======
final int dataSize = 181;
final int maxDataIdx = dataSize-1;
float[] freqData = new float[dataSize];
int[] binData = new int[dataSize];

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

final float xStep = guiCanvasWidth/maxDataIdx;


void settings() {
  size((int)(guiCanvasWidth+2*guiCanvasHorizontalOffset),
       (int)(guiCanvasHeight+2*guiCanvasVerticalOffset));
}


void setup() {
  ellipseMode(CENTER);
  
  // initialize the LUTs with the minimum of each parameter
  for (int i = 0; i < dataSize; i++) {
    freqData[i] = freqMin;
    binData[i] = binMin;
  }
  
  // Since this sketch calculates the LUTs only once we can stop
  // after the first iteration of draw().
  noLoop();
}


void draw() {
  //====== generate canvas ======
  background(guiBackgroundColor);
  strokeWeight(10);
  stroke(guiCanvasColor);
  fill(guiCanvasColor);
  rect(guiCanvasHorizontalOffset, guiCanvasVerticalOffset, guiCanvasWidth, guiCanvasHeight);
  strokeWeight(0);
  translate(guiCanvasHorizontalOffset, guiCanvasVerticalOffset);
  
  //====== generate parametrization curves ======
  switch (function) {
    case Continuous:
      calcAndDrawContinuous();
      break;
    case Step:
      calcAndDrawStep();
      break;
    case StepsPerStep:
      calcAndDrawStepsPerStep();
      break;
  }
  
  //====== get the LUTs from console ======
  printBinLUT();
  printFreqLUT();
}


void calcAndDrawStepsPerStep() {
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
    //====== draw frequency graph (sawtooth function) ====== 
    freqX = idx * xStep;
    if (idx%(int(0.5+(binsPerStep))) == 0) {
      freqY = guiCanvasHeight;
    } else  {
      freqY -= (idx%(int(0.5+(binsPerStep/freqLevels))) != 0) ? 0 : freqYstep;
    }
    strokeWeight(1);
    stroke(guiFrequencyColor);
    line(lastFreqX, lastFreqY, freqX, freqY);
    lastFreqX = freqX;
    lastFreqY = freqY;
    strokeWeight(0);
    fill(guiFrequencyColor);
    circle(freqX, freqY, guiCircleSize2);
    
    //====== draw bin graph (step function) ====== 
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
    
    //====== set data in LUTs ======
    freqData[idx] = round(map(freqY, 0, guiCanvasHeight, freqMax, freqMin));
    binData[idx] = (int)(0.5+map(binY, 0, guiCanvasHeight, binMax, binMin));
  }
}


void calcAndDrawContinuous() {
  final float binYstep = guiCanvasHeight / dataSize;
  float binX = 0;
  float binY = guiCanvasHeight;
  float lastBinX = binX;
  float lastBinY = binY;
  
  final float periode = dataSize / periodeSteps;
  final float periode2 = 2 * periode;
  final float amplitude = (guiCanvasHeight / periode);
  float freqX = 0;
  float freqY = 0;
  float lastFreqX = freqX;
  float lastFreqY = freqY;
  for (int idx = 0; idx < dataSize; idx++) {
    //====== draw frequency graph ====== 
    freqX = idx * xStep;
    switch(waveForm) {
      case Triangle:
        freqY = (periode - abs((idx % periode2) - periode)) * amplitude;
        break;
      case TriangleInverse:
        freqY = (abs((idx % periode2) - periode)) * amplitude;
        break;
      case Sawtooth:
        freqY = (abs((idx % periode) - periode)) * amplitude;
        break;
      case SawtoothInverse:      
        freqY = (periode - abs((idx % periode) - periode)) * amplitude;
        break;
    }
    strokeWeight(1);
    stroke(guiFrequencyColor);
    line(lastFreqX, lastFreqY, freqX, freqY);
    lastFreqX = freqX;
    lastFreqY = freqY;
    strokeWeight(0);
    fill(guiFrequencyColor);
    circle(freqX, freqY, guiCircleSize2);
    
    //====== draw bin graph (linear function) ======
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
    
    //====== set data in LUTs ======
    freqData[idx] = round(map(freqY, 0, guiCanvasHeight, freqMax, freqMin));
    binData[idx] = (int)(0.5+map(binY, 0, guiCanvasHeight, binMax, binMin));
  }
}


void calcAndDrawStep() {
  final float binsPerStep = dataSize / steps;
  final float binYstep = guiCanvasHeight / (steps-1);
  float binX = 0;
  float binY = guiCanvasHeight;
  float lastBinX = binX;
  float lastBinY = binY;
  
  final float periode = (waveForm == Waveform.Triangle || waveForm == Waveform.TriangleInverse) ? (binsPerStep/2) : binsPerStep;
  final float periode2 = 2*periode;
  final float amplitude = (guiCanvasHeight/periode);
  float freqX = 0;
  float freqY = 0;
  float lastFreqX = freqX;
  float lastFreqY = freqY;
  for (int idx = 0; idx < dataSize; idx++) {
    //====== draw frequency graph ====== 
    freqX = idx * xStep;
    switch(waveForm) {
      case Triangle:
        freqY = (periode - abs((idx % periode2) - periode)) * amplitude;
        break;
      case TriangleInverse:
        freqY = (abs((idx % periode2) - periode)) * amplitude;
        break;
      case Sawtooth:
        freqY = (abs((idx % periode) - periode)) * amplitude;
        break;
      case SawtoothInverse:      
        freqY = (periode - abs((idx % periode) - periode)) * amplitude;
        break;
    }
    strokeWeight(1);
    stroke(guiFrequencyColor);
    line(lastFreqX, lastFreqY, freqX, freqY);
    lastFreqX = freqX;
    lastFreqY = freqY;
    strokeWeight(0);
    fill(guiFrequencyColor);
    circle(freqX, freqY, guiCircleSize2);
    
    //====== draw bin graph (step function) ====== 
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
    
    //====== set data in LUTs ======
    freqData[idx] = round(map(freqY, 0, guiCanvasHeight, freqMax, freqMin));
    binData[idx] = (int)(0.5+map(binY, 0, guiCanvasHeight, binMax, binMin));
  }
}


void printBinLUT() {
  println("Bin LUT:");
  print("{");
  for (int i = 0; i < dataSize; i++) {
    print(binData[i]);
    print((i != maxDataIdx) ? ", " : "}\n");
  }
}


void printFreqLUT() {
  println("Frequency LUT:");
  print("{");
  for (int i = 0; i < dataSize; i++) {
    print(freqData[i]);
    print((i != maxDataIdx) ? ", " : "}\n");
  }
}
  
