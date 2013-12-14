import system.io;
package robot;

abstract class Robot 
{
    void rotate(int angle);
}

class LifeRobot implements Robot 
{
    public void construct(string name);
    public void destruct();
    public void rotate(int angle);
    private string name;
    private int price; 
}

public class MyRobot extend LifeRobot
{
    public void rotate(int angle);
   
    public int getPrice() {
        return self.price;
    }
    
    public int getRotation(); 
    public int setPrice(int price) {
        self.price = price;
    }
    public void setName(string name) {
        self.name = name;
    }
}

class RobotFactory
{
    public static int main(string options[], int count) {
        MyRobot robot;
        robot.setName("jenson");
        robot.rotate(30);
    }
}
