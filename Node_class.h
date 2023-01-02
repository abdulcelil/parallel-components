/* ---------------------------------------------------------------------
 * Parallel Components Thread Container Library
 * Copyright (C) 2022 ,Abdulcelil DOÐAN.  Unless you have an agreement
 * with Abdulcelil DOÐAN, for a separate license for this software code, the
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
#pragma once
#include <functional>
#include <iostream>
#include <thread>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <type_traits>
#include <string>
#include <algorithm>

//This is the global mutex for all components
std::mutex mutex{};

//Test auxiliary function to check whether all queues at the output of a component are equal in size.
//The purpose of this function is to distribute all output queues equally.
template<typename T>
bool queue_equality(const std::vector<std::queue<T>>& queue_vec)
{

    for (int i = 0; i < queue_vec.size() - 1; i++)
    {

        if (queue_vec.at(i).size() != queue_vec.at(i+1).size())
        {

            return false;
        }


    }
    return true;
}



//Allows the components to be packaged ,to be prepared for receiving their packages.
template <class Tuple, std::size_t... I>
void send_ready(Tuple& t, std::index_sequence<I...>)
{

    (std::get<I>(t).send_order_no(I) && ...);

}

template <typename ReturnType, typename... FunctionParamTypes>
class Node
{
private:

    //The number of subsequently started components that the component will pack
    int con_number{};
    
    //thread start flag ,default true 
    bool start_flag{ true };
    
    //vector to hold the thread, threads can only be kept in this way, because otherwise it must be defined beforehand
    std::vector<std::thread> threads{};
    
    //lambda function object that threads will run
    std::function<ReturnType(FunctionParamTypes...)> function;
    
    //determines in which queue the packet will be delivered
    int send_order{0};
    
    //output queues of the component
    std::vector<std::queue<ReturnType>> queue_vec;
public:
   
    //constructor
    Node<ReturnType, FunctionParamTypes...>(int con_number_, const std::function<ReturnType(FunctionParamTypes...)>& function_)
        : con_number(con_number_), function(function_)
    {

        queue_vec.resize(con_number_);
    }

    //Thread function
    //Takes as parameters the component objects whose packages it will receive
    template <typename... OtherCallingNodes>
    void run(OtherCallingNodes& ...other_calling_nodes)
    {
        while (start_flag)
        {
                //To prevent simultaneous reception of other components
                mutex.lock();

                
                using Tuple = std::tuple<OtherCallingNodes &...>;
                Tuple tuple(other_calling_nodes...);

                constexpr auto size = std::tuple_size_v<std::remove_reference_t<Tuple>>;
                auto mki = std::make_index_sequence<size>{};
                
                //It is requested to prepare the components whose packages will be taken.
                send_ready(tuple, mki);

                //If the output queues are empty or have equal sizes, start the reception.
                if (queue_equality<ReturnType>(queue_vec))
                {
                    //Start package recevive if other components are ready
                    if ((other_calling_nodes.get_ready() && ...))
                    {
                        //Receive and process packages
                        ReturnType  r = function(other_calling_nodes.get_packet() ...);
                        
                        //Accept if lambda result is true
                        if (r.get())
                        {
                            //Push function result to output queues
                            for (std::queue<ReturnType>& q : queue_vec)
                            {
                                q.push(r);
                            }
                            //Data received and other components stop sending until next reception
                             ((other_calling_nodes.send_stop() && ...));


                        }
                    }
                }

                
               
                mutex.unlock();


        }

    }
    
    //Queue number to start the sends
    bool send_order_no(int order)
    {
        
        send_order = order;
        return true;

    }

    //Cancel package delivery until next pick up
    bool send_stop()
    {
        
        send_order = -1;
        
        
        return true;

    }

    //send package from requested queue
    ReturnType get_packet()
    {

        if (get_ready())
        {
            ReturnType res = queue_vec.at(send_order).front();
            queue_vec.at(send_order).pop();
            return res;
        }
        return ReturnType();
    }

   // Is the package ready to send?
    bool get_ready() const
    {
       if( send_order > -1 && send_order <= con_number - 1 && send_order < queue_vec.size() && !queue_vec.at(send_order).empty())
        {
            return true;
        }

        return false;

    }


    //the function that will trigger the thread and pass the components to the run function
    template <typename... OtherCallingNodes>
    void start(OtherCallingNodes& ...other_calling_nodes)
    {

        threads.push_back(std::thread(&Node<ReturnType, FunctionParamTypes...>::run<OtherCallingNodes&...>, this, std::ref(other_calling_nodes) ...));
    }

    void join()
    {
        if (threads.at(0).joinable())
            threads.at(0).join();
    }
};

