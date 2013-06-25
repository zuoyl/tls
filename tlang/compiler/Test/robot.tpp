include "robot.th"
class InternalRobot extend MyRobot
{

}

void LifeRobot::construct(string name)
{
    self.name = name;
}

void LifeRobot::destruct()
{
}

void LefeRobot::rotate(int angle)
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
    self.price += 30;
    if (self.price > 50)
        self.price = 50;
}
