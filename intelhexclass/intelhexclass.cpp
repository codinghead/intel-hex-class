#include <vector>
#include <map>

class queue {
    private:
        static int data[STACK_SIZE];
        static unsigned int count;
        static unsigned int putPointer;
        static unsigned int getPointer;
        static bool queueLocked;
        bool useQueue;
        
    public:
        // Initialise starting parity value in constructor
        queue() {useQueue = false;}
        
        // Use automatically generated copy constructor
        // queue(const parity& old_parity)
        
        // Use automatically generated assignment operator
        // queue operator = (const queue& old_queue)
        
        // Use automatically generated deconstrutor
        // ~queue()
        void put(int item);
        int get(void);
        bool lockQueue(void);
        bool unlockQueue(void);
};
