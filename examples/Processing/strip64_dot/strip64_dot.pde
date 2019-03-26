OPC opc;
PImage dot;

void setup()
{
  size(800, 200);

  // Load a sample image
  dot = loadImage("color-dot.png");

  // Connect to the local instance of fcserver
  opc = new OPC(this, "192.168.1.14", 7890);

  // Map one 64-LED strip to the center of the window
  opc.ledStrip(0, 60, width/2, height/2, width / 70.0, 0, false);
}

void draw()
{
  background(0);

  // Draw the image, centered at the mouse location
  float dotSize = width * 0.2;
  image(dot, mouseX - dotSize/2, mouseY - dotSize/2, dotSize, dotSize);
}