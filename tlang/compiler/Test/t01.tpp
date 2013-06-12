include "t10.th"

protocol Robot 
{
    void rotate(int angle);
}

class LifeRobot implements Robot 
{
    public void construct(string name);
    public void destruct();
    public void rotate(int angle);
}

public class MyRobot extend LifeRobot
{
    public void rotete(int angle);
    public int getID();
    public int getRotation();
    public int setID(int id);
}

class RobotFactory
{
    public static int main(set<string> options)
    {
        string name = "myrobot";
        MyRobot robot = new MyRobot(name);
        robot.rotete(30);
    }
}



