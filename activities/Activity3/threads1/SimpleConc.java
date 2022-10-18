
import java.io.StringWriter;

class SimpleConc implements Runnable
{
    int thread_id;    // Variable containing specific id of this thread.
    StringWriter outstream;

    // Run: overides Runnabale.Run, thread entry point
    public void run ()
    {
    this.outstream.write( thread_id + ": Running thread\n" );
    }

    // Constructor: set thread id
    SimpleConc ( int id, StringWriter outstream )
    {
        this.thread_id = id;
        this.outstream = outstream;
    }

}