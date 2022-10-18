import java.io.StringWriter;


class Main 
{
    public static void main(String[] args) 
    {
        int numthreads = 8;

        StringWriter outstream = new StringWriter();

        // create and start specified thread objects of class SimpleConc
        for ( int i=1; i<=numthreads; i++ )
        {
            new Thread ( new SimpleConc(i, outstream) ).start();
        }

        System.out.println(outstream);
        
    }
}