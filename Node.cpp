/* ---------------------------------------------------------------------
 * Parallel Components Thread Container Library
 * Copyright (C) 2022 ,Abdulcelil DOĞAN.  Unless you have an agreement
 * with Abdulcelil DOĞAN, for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * https://abdulcelil.com.tr/
 * abdulcelildogan@gmail.com
 * ----------------------------------------------------------------------
 */

#include "Node_class.h"


using StringPtr = std::shared_ptr<std::string>;

template <typename ReturnType>
class Input
{
private:

    bool start_flag{ true };
    std::vector<std::thread> threads{};
    std::function<ReturnType(void)> function;
    int send_order{ 0 };
    std::queue<ReturnType> q;
    bool ready_flag{ false };

public:

    Input<ReturnType>( const std::function<ReturnType(void)>& function_)
        : function(function_)
    {
    }
   
    bool consumer_number_inc()
    {
        return true;
    }


    void run()
    {

        while (start_flag)
        {
           
            if (ready_flag)
            {
                mutex.lock();
                StringPtr str = function();
                if (str.get())
                {
                    q.push(str);
                }
                mutex.unlock();
            }
           

        }
        

    }
    bool producer_get_order(int order)
    {

        send_order = order;
        return true;

    }

    bool producer_stop()
    {
        //Butun beslenen nodelar elemanlarını aldığında dursun

        send_order = -1;


        return true;

    }


    ReturnType producer_get_packet()
    {

        if (producer_get_ready())
        {
            ReturnType res = q.front();
            q.pop();
            return res;
        }
        return ReturnType();
    }


    bool producer_get_ready() const
    {
        if (send_order > -1 && !q.empty())
        {
            return true;
        }

        return false;

    }

    void handle()
    {

        threads.push_back(std::thread(&Input<ReturnType>::run, this));
    }

    void start()
    {
     
        ready_flag = true;
    

    }
    void join()
    {
        if (threads.at(0).joinable())
            threads.at(0).join();
    }
};


int main()
{

    int m{};

  
    std::mutex _mutex;
    int i = 0;

    Input<StringPtr> input( [&]()
        {
           
                return  std::make_shared<std::string>(std::to_string(i++));

        });

    Node<StringPtr, StringPtr> f1([&](const StringPtr a)
        {
            if (a.get())
            {
                //std::cout << m << "--" << "F1:" << "f1(" + *a + ")" << std::endl;

                return  std::make_shared<std::string>("f1(" + *a + ")");
            }
    return StringPtr();

        });

    Node<StringPtr, StringPtr> f2([&](const StringPtr a)
        {
            if (a.get())
            {
                _mutex.lock();
                std::cout << m << "--" << "F2:" << "f2(" + *a + ")" << std::endl;
                _mutex.unlock();

                return  std::make_shared<std::string>("f2(" + *a + ")");
            }
    return StringPtr();

        });


    Node<StringPtr, StringPtr, StringPtr> f3([&](const StringPtr a, const StringPtr b)
        {

            if (a.get() && b.get())
            {
                return std::make_shared<std::string>("f3(" + *a + "," + *b + ")");

            }
    return StringPtr();

        });
    Node<StringPtr, StringPtr, StringPtr> f4([&](const StringPtr a, const StringPtr b)
        {

            if (a.get() && b.get())
            {
                return std::make_shared<std::string>("f4(" + *a + "," + *b + ")");

            }
           
    return StringPtr();

        });

    input.handle();
    f1.handle(input);
    f2.handle(f1);
    f3.handle(f2, f1);
    f4.handle(f3, f2);
    f4.consumer_number_inc();


    input.start();
    f1.start();
    f2.start();
    f3.start();
    f4.start();

    while (true)
    {
        mutex.lock();
        StringPtr res = f4.producer_get_packet();
        if (res.get())
            std::cout << m << "--" << "Result:" << *res << std::endl;

        mutex.unlock();
        m++;

    }

    input.join();
    f1.join();
    f2.join();
    f3.join();
    f4.join();

}


