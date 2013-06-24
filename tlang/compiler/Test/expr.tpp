/// test source file for expr 
public class Object
{
   public void construct(const string name);
   public void destruct();
   public string getName();
   public void  setName(const string name);
   public void print();
   private string name;
   private bool printable;
}

class Application
{
    public static void main(set<string> options);
}

void Application::main(set<string> options)
{
    Object obj("jenson");
    string name = obj.getName();
    obj.setName("zuoyl");
    name = obj.getName();
    obj.print();
}





