// Implementation of a subset of the java.util.concurrent.BlockingQueue interface 

// Implementation inspired from 
// https://www.happycoders.eu/algorithms/implement-queue-using-array/

public class BlockingQueue {

    // queue and dequeue string data -- not objects -- makes it easier to read
    private String [] queue;

    // queue metadata  
    private int limit = 10;
    private int head = 0;
    private int tail = 0;
    private int qlen = 0;

    // Create an array of strings as the queue
    public BlockingQueue(int limit){
        this.limit = limit;
        this.queue = new String [limit];
    }

    public synchronized void put(String item)
    throws InterruptedException  {
    
        // variable for slot item goes in
        int slot;

        // wait and don't add if the queue is full
        // if (this.qlen==this.limit)
        while (this.qlen==this.limit)
        {
            wait();
        } 

        // get slot and update head and length
        slot = this.tail;
        this.tail = this.tail + 1;
        if (this.tail == this.limit) //Circular indexes
        {
            this.tail = 0;
        } 
        this.qlen += 1; 

        // notify takers if this is the first item in queue
        if(this.qlen == 1) 
        {
            notifyAll();
        }

        // add the item
        this.queue[slot] = item;
    }

    public synchronized String take()
    throws InterruptedException {

        // slot to be taken and deleted
        int slot;

        // don't take from an empty queue
        // if (this.qlen==0)
        while(this.qlen==0)
        {
            wait();
        } 

        //get slot 
        slot = this.head;
        this.head= this.head+ 1;
        if (this.head== this.limit) //Circular indexes
        {
            this.head= 0;
        } 

        // if taking from a full queue, notify putters
        if(this.qlen == this.limit)
        {
            notifyAll();
        }
        // update queue length
        this.qlen -= 1; 

        // take the item and dereference pointer for garbage collection
        String ret_obj = this.queue[slot];
        queue[slot]=null;

        // return item
        return ret_obj;
    }
}
