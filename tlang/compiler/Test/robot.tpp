include "robot.th"


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
    public int getPrice();
    public int getRotation();
    public int setPrice(int price);
    public void setName(string name);
}

class RobotFactory
{
    public static int main(set<string> options);
}

class InternalRobot extend MyRobot
{

}

void LefeRobot::construct(string name)
{
    self.name = name;
}

void LifeRobot::destruct()
{
}

void LifeRobot::rotate(int angle)
{

}

void MyRobot::rotate(int angle)
{

}

int MyRobot::getPrice()
{
    return self.price;
}

void MyRobot::setPrice(int price)
{
    self.price = price;
}
void MyRobot::setName(string name)
{
    self.name = name;
}
