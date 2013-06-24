/// test souce for class

public class GraphicDeviceContext
{
    public void construct(int handle);
    public void destruct();
    public int getHandle(); 
    private int handle;
}


public abstract class Graphic
{
    public void draw(GraphicDeviceContext context);
    public void move(int x, int y);
    public void select(int x, int y);
}

public class Rectange implments Graphic
{
    public void draw(GraphicDeviceContext context);
    public void move(int x, int y);
    public void select(int x, int y);
    /// attribute
    int x;
    int y;
    int height;
    int width;
}

public class Triange implements Graphic 
{
    public void draw(GraphicDeviceContext context);
    public void move(int x, int y);
    public void select(int x, int y);
}

class Application
{
    static void main(set<string> options);
}


void Application::main(set<string> options)
{
   set<Graphic> graphics;
   GraphicDeviceContext context;
   Rectangle rectangle = new Rectangle();
   Triangle  triangle = new Triangle();
   graphics.add(rectange);
   graphics.add(triange);
   for ( Graphic g in graphics)
       g.draw(context);
}
