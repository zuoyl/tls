#include "robot.h"


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
