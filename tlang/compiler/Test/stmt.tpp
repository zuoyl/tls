/// test source file for stmt 
public class TestCase 
{
   public void construct();
   public void destruct();
   public void testIfStmt();
   public void testWhileStmt();
   public void testForStmt();
   public void testForEachStmt();
   public void testDoStmt();
   public void testNewStmt();
   public void testReturnStmt();
   public void testAssertStmt();
   public void testBreakStmt();
   public void testSwichStmt();
   public void testTryStmt();
   public void testCatchStmt();
   public void testExprStmt();
   public void testContinueStmt();
   private string author;
}

class Tester 
{
    public static void main(set<string> options);
}

void Tester::main(set<string> options)
{
    TestCase m;
    m.testIfStmt();
    m.testWhileStmt();
    m.testForStmt();
    m.testForEachStmt();
    m.testDoStmt();
    m.testNewStmt();
    m.testBreakStmt();
    m.testAssertStmt();
    m.testReturnStmt();
    m.testContinueStmt();
    m.testTryStmt();
    m.testCatchStmt();
    m.testExprStmt();
}

void TestCase::construct()
{
    self.author = "jenson.zuo@gmail.com";
}
void TestCase::destruct()
{
}
void TestCase::testIfStmt()
{
    int index = 0;
    index += 10;

    if (index > 10)
        index = 10;
    else
        index += 10;

    if (index > 10) {
        index = 10;
        index += 20;
    }
    else {
        index += 10;
    }

}
void TestCase::testWhileStmt()
{
    int index = 0;

    while (index < 10)
        index += 1;
    while (index > 0) {
        index -= 1;
        if (index == 5)
            break;
    }
}
void TestCase::testForStmt()
{
    int index;
    int sum = 0; 
    
    for (index = 0; index < 10; index += 1)
        sum += index;

    for (int i = 0; i < 10; i += 1) {
        sum += i;
    }
}
void TestCase::testForEachStmt()
{

}
void TestCase::testDoStmt()
{

}
void TestCase::testNewStmt()
{

}
void TestCase::testBreakStmt()
{

}
void TestCase::testContinueStmt()
{

}
void TestCase::testAssertStmt()
{

}
void TestCase::testTryStmt()
{

}
void TestCase::testCatchStmt()
{

}
void TestCase::testReturnStmt()
{

}



