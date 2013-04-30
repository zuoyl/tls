import sample.class1.class2;
import sample.class2.class3;

public set<int> set1 = [1, 2, 3];
public set<int> set2 = [2, 3, 4];

public map<int, string> map1 = { 1:"hello", 2:"sample", 3:"world"};
public map<int string>  map2 = {1:"hello"};

int globalVar1;
int globalVar2 = 1 + 1;

public type Rect {
    int x;
    int y;
    set<int> set1;

}

public int robotNumber = 10;

int getRoboteNumber()
{
    int result = 0;
    set<int> set3 = set1 + set2;
    foreach (int i in set3) {
        result += i;
    }
    
    foreach (int i in [0,1,2]) {
        result +=i;
    }
    
    // foreach(key, val in map2)
    foreach (int key, string val in map2) {
        print(key);
        print(value);
    }
    foreach (int key, string val in {1:"hello", 2: "world"})
    
    returnRoboteNumber;
}

protocol Robot {
    void rotate(int angle);
}

class LifeRobot implement Robot {
    void LifeRobot(string name)
    void LieftRobotDestroy();
    public void rotate(int angle);
}

public class MyRobot extend LifeRobot
    implement OtherPrototol1, OtherPototol2 {
    public void rotete(int angle);
    public int getID();
    public int getRotation();
    public int setID(int id);
}

int main() {
    string name = "myrobot";
    MyRobot robot = new MyRobot(name);
    robot.rotete(30);
}



