public class Test {
   public static void main(String[] args) {
    java.util.List<String> list = new java.util.ArrayList<String>();
    for (int i = 0; i < 5; i++) {
      String newString = args[0] + i;
      list.add(newString);
    }
    System.out.println(list);
  }
}
