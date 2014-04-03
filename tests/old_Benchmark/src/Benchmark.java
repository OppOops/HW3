public class Benchmark{
    public static void main(String[] args){
        final int iter1 = 1000;
        final int iter2 = 1000;
        final int iter3 = 100;
        int counter = 0;
        long startTime = java.lang.System.currentTimeMillis();
        for(int i = 0;i<iter1;i++){
            for(int j = 0;j<iter2;j++){
                for(int k = 0;k<iter3;k++){
                    counter++;
                }
            }
        }
        long endTime = java.lang.System.currentTimeMillis();
        int elapsedTime = (int)(endTime - startTime);
        System.out.println("BenchMark result: " + elapsedTime + "ms. After " + counter + " iterations.");
    }
}
