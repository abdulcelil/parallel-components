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

std::mutex mutex{};
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




template <class Tuple, std::size_t... I>
void send_ready(Tuple& t, std::index_sequence<I...>)
{

    (std::get<I>(t).send_order_no(I) && ...);

}

template <typename ReturnType, typename... FunctionParamTypes>
class Node
{
private:

    int con_number{};
    bool start_flag{ true };
    std::vector<std::thread> threads{};
    std::function<ReturnType(FunctionParamTypes...)> function;
    int send_order{0};
    std::vector<std::queue<ReturnType>> queue_vec;
public:
    



    Node<ReturnType, FunctionParamTypes...>(int con_number_, const std::function<ReturnType(FunctionParamTypes...)>& function_)
        : con_number(con_number_), function(function_)
    {

        queue_vec.resize(con_number_);
    }


    template <typename... OtherCallingNodes>
    void run(OtherCallingNodes& ...other_calling_nodes)
    {
        while (start_flag)
        {
                
                mutex.lock();

                
                using Tuple = std::tuple<OtherCallingNodes &...>;
                Tuple tuple(other_calling_nodes...);

                constexpr auto size = std::tuple_size_v<std::remove_reference_t<Tuple>>;
                auto mki = std::make_index_sequence<size>{};
                
                send_ready(tuple, mki);


                if (queue_equality<ReturnType>(queue_vec))
                {
                    if ((other_calling_nodes.get_ready() && ...))
                    {
                        ReturnType  r = function(other_calling_nodes.get_packet() ...);

                        if (r.get())
                        {
                            for (std::queue<ReturnType>& q : queue_vec)
                            {
                                q.push(r);
                            }

                             ((other_calling_nodes.send_stop() && ...));


                        }
                    }
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
            ReturnType res = queue_vec.at(send_order).front();
            queue_vec.at(send_order).pop();
            return res;
        }
        return ReturnType();
    }

   
    bool get_ready() const
    {
       if( send_order > -1 && send_order <= con_number - 1 && send_order < queue_vec.size() && !queue_vec.at(send_order).empty())
        {
            return true;
        }

        return false;

    }


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

