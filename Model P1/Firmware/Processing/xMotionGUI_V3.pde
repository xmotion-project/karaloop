// ------------------------------------------------------------------------------------
// libraries
import grafica.*;
import controlP5.*;
import processing.serial.*;

// ------------------------------------------------------------------------------------
// global variables
int seuil1 = 2000;
int seuil2 = 1000;
int seuil3 = 3000;
int seuil6 = 3000;

// Graphs
int nPoints = 500;
int Points = 400;
int inc = 0;
long previousMillis = 0;
int duration = 5;

// Tabs
boolean tab_control = false;
boolean tab_default = false;

// Analogue input
int A0;
int A1;
int A2;
int A5;
String [] sensor = new String [4];

// serial event
boolean newData = false;
String inString;  // Input string from serial port


// objects
ControlP5 cp5;
CheckBox checkbox;
GPointsArray points;
GPlot[] plots = new GPlot[6];
Serial myPort;    // The serial port

// ------------------------------------------------------------------------------------
void setup()
{
  size(1500, 900);
  fill(80);
  textSize(50);
  background(255);

  myPort = new Serial(this, "COM14", 9600);
  myPort.bufferUntil('\n'); 
  println((Object[])Serial.list()); // Imprime liste ports actifs 

  cp5 = new ControlP5(this);
  points = new GPointsArray(nPoints);

  // Change the actual font
  PFont pfont = createFont("Arial", 10, true); // use true/false for smooth/no-smooth
  ControlFont font = new ControlFont(pfont, 25);

  // Add a new tab
  cp5.addTab("Control")
    .setHeight(50) 
    .setWidth(150)
    .getCaptionLabel()
    .setFont(font)
    ;

  // Modify tab
  cp5.getTab("default")
    .setLabel("Visualization")
    .activateEvent(true)
    .setId(1)
    .setHeight(50)
    .setWidth(210)
    .getCaptionLabel()
    .setFont(font)
    ;

  cp5.getTab("Control")
    .activateEvent(true)
    .setId(2)
    ;

  // Graphs initializing
  Init_graph();
}

// ------------------------------------------------------------------------------------
void draw()
{

  if (tab_default == true)
  {
    background(255);
    stroke(0);
    fill(0);
    textSize(15);

    // affichage du texte : seuil
    text("THRESOLD", 650, 80);
    text("THRESOLD", 1350, 80);

    noFill();
    strokeWeight(1);
    rect(652, 120, 60, 21, 5);
    rect(652, 370, 60, 21, 5);
    rect(652, 620, 60, 21, 5);

    rect(1350, 120, 60, 21, 5);
    rect(1350, 370, 60, 21, 5);
    rect(1350, 620, 60, 21, 5);

    fill(0);

    text(String.format("%d", seuil1), 665, 137);
    text(String.format("%d", seuil2), 665, 387);
    text(String.format("%d", seuil3), 665, 637);
    text(String.format("%d", seuil6), 1363, 637);


    //affichage des indicateurs de seuil
    strokeWeight(2);
    if (A0>seuil1) {
      fill(0, 255, 0);
    } else {
      noFill();
    }
    ellipse(682, 180, 40, 40); //P1
    if (A1>seuil2) {
      fill(0, 255, 0);
    } else {
      noFill();
    }
    ellipse(682, 430, 40, 40); //P2
    if (A2>seuil3) {
      fill(0, 255, 0);
    } else {
      noFill();
    }
    ellipse(682, 680, 40, 40); //P3
    if (A5>seuil6) {
      fill(0, 255, 0);
    } else {
      noFill();
    }
    ellipse(1382, 680, 40, 40); //P4

    //affichage des graphs
    for (int i = 0; i<6; i++)
    {    
      plots[i].beginDraw();
      plots[i].drawBackground();
      plots[i].drawBox();
      plots[i].drawXAxis();
      plots[i].drawYAxis();
      plots[i].drawTopAxis();
      plots[i].drawRightAxis();
      plots[i].drawTitle();
      plots[i].getMainLayer().drawPoints();
      plots[i].endDraw();
    }

    // affichage des lines correspondant aux seuils définis  
    float offset_seuil1 = map(seuil1, 0, 4095, 315, 115);
    float offset_seuil2 = map(seuil2, 0, 4095, 565, 365);
    float offset_seuil3 = map(seuil3, 0, 4095, 815, 615);
    float offset_seuil6 = map(seuil6, 0, 4095, 815, 615);

    strokeWeight(4);
    line(120, offset_seuil1, 620, offset_seuil1);
    line(120, offset_seuil2, 620, offset_seuil2);
    line(120, offset_seuil3, 620, offset_seuil3);
    line(820, offset_seuil6, 1320, offset_seuil6);
    //print(mouseX);print(";");println(mouseY);

    if (inc>nPoints)
    {
      inc=0;
    }

    // Add and remove point the point  
    if ( millis() > previousMillis + duration) 
    {   
      inc++;

      if (newData) {
        plots[0].addPoint(inc, A0);
        plots[0].removePoint(0);

        plots[2].addPoint(inc, A2);
        plots[2].removePoint(0);

        plots[1].addPoint(inc, A1);
        plots[1].removePoint(0);

        plots[5].addPoint(inc, A5);
        plots[5].removePoint(0);

        newData = false;
      }
      previousMillis = previousMillis + duration;
    }
  }
}

// ------------------------------------------------------------------------------------
// Init_graph : Initializing graphics
//-------------------------------------------------------------------------------------
void Init_graph() {
  int espacementX = 0;
  int espacementXinit = 50;
  int espacementY = 0;
  int espacementYinit = 75;

  // calculate initial display points
  for (int i = 0; i < Points; i++)
  {
    points.add(i, 0);
  }

  for (int i = 0; i<6; i++)
  {
    plots[i] = new GPlot(this); 

    if (i>2) {
      espacementX = espacementXinit + 700;
    } else {
      espacementX = espacementXinit;
    }

    if (i == 1 || i == 4) {
      espacementY = espacementYinit + 250;
    } else if (i == 2 || i == 5) {
      espacementY = espacementYinit + 500;
    } else {
      espacementY = espacementYinit;
    }

    // Create a new plot and set its position on the screen
    plots[i].setPos(espacementX, espacementY);

    // Set the plot title and the axis labels
    plots[i].setTitleText("Probe " + str(i+1));

    //plot.getXAxis().setAxisLabelText("x axis");
    plots[i].getYAxis().setAxisLabelText("N bits");

    plots[i].setXLim(0, nPoints);   // set x limits
    plots[i].setYLim(0, 4095); // set y limits

    plots[i].setDim(500, 200); // set dimension of the plot

    // Add the two set of points to the plot
    plots[i].setPoints(points);
  }
}

// ------------------------------------------------------------------------------------
// controlEvent : Get informations of CP5 
//-------------------------------------------------------------------------------------
void controlEvent(ControlEvent theControlEvent) {

  // Tab events
  if (theControlEvent.isTab()) {
    String tab = theControlEvent.getTab().getName();  

    // Tab display
    if (tab == "default") {

      tab_default = true;
      tab_control = false;
    }
    if (tab =="Control")
    {
      tab_control = true;
      tab_default = false;
      background(255);
    }
  }
}

// ------------------------------------------------------------------------------------
// SerialEvent
//-------------------------------------------------------------------------------------
void serialEvent(Serial myPort) {

  inString = myPort.readString(); // get new value from serial port

  if (inString != null) { 

    sensor = inString.trim().split(";"); //split and remove withespace from string

    // analogue channel assignement
    A0 = int(sensor[0]);
    A1 = int(sensor[1]);
    A2 = int(sensor[2]);
    A5 = int(sensor[3]);

    newData = true;
  }
} 

/*
// ------------------------------------------------------------------------------------
 // Test de connection entre le dispositif et le programme
 //-------------------------------------------------------------------------------------
 boolean connect( int limit){
 int tries = 0;
 while(tries<limit){
 try{
 myPort = new Serial(this, Serial.list()[tries], 9600);
 }catch(Exception e){
 tries++;
 continue;
 }
 break;
 }
 if(tries<limit){
 println("Successsful in" + str(tries+1) + " tries.");
 return true;
 }else{
 System.err.println("Unscucessful. Reached limit.");
 return false;
 }
 }*/
