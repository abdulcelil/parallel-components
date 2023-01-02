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

    int con_number{};
    bool start_flag{ true };
    std::vector<std::thread> threads{};
    std::function<ReturnType(void)> function;
    int send_order{ 0 };
    std::queue<ReturnType> q;
    int i{};
public:

    Input<ReturnType>(int con_number_, const std::function<ReturnType(void)>& function_)
        : con_number(con_number_), function(function_)
    {
    }
   
    void run()
    {
        while (start_flag)
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
    bool send_order_no(int order)
    {

        send_order = order;
        return true;

    }

    bool send_stop()
    {
        send_order = -1;
        return true;
    }


    ReturnType get_packet()
    {

        if (get_ready())
        {
            ReturnType res = q.front();
            q.pop();
            return res;
        }
        return ReturnType();
    }


    bool get_ready() const
    {
        if (send_order > -1 && !q.empty())
        {
            return true;
        }

        return false;

    }

    void start()
    {

        threads.push_back(std::thread(&Input<ReturnType>::run, this));
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
    int i{}
    std::mutex _mutex;
    
    Input<StringPtr> input(1, [&]()
        {
           
                return  std::make_shared<std::string>(std::to_string(i++));

        });

    Node<StringPtr, StringPtr> f1(2, [&](const StringPtr a)
        {
            if (a.get())
            {
                _mutex.lock();
                std::cout << m << "--" << "F1:" << "f1(" + *a + ")" << std::endl;
                _mutex.unlock();

                return  std::make_shared<std::string>("f1(" + *a + ")");
            }
    return StringPtr();

        });

    Node<StringPtr, StringPtr> f2(2, [&](const StringPtr a)
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


    Node<StringPtr, StringPtr, StringPtr> f3(1, [&](const StringPtr a, const StringPtr b)
        {

            if (a.get() && b.get())
            {
                return std::make_shared<std::string>("f3(" + *a + "," + *b + ")");

            }
    return StringPtr();

        });
    Node<StringPtr, StringPtr, StringPtr> f4(1, [&](const StringPtr a, const StringPtr b)
        {

            if (a.get() && b.get())
            {
                return std::make_shared<std::string>("f4(" + *a + "," + *b + ")");

            }
    return StringPtr();

        });

    input.start();
    f1.start(input);
    f2.start(f1);
    f3.start(f2, f1);
    f4.start(f3, f2);



    while (true)
    {
        mutex.lock();
        StringPtr res = f4.get_packet();
        if (res.get())
            std::cout << m << "--" << "Result:" << *res << std::endl;

        mutex.unlock();
        m++;

    }
    f1.join();
    f2.join();
    f3.join();
    f4.join();

}


