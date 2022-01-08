#include <list>
#include <deque>
#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "boolinq.h"

using namespace boolinq;

// Where Tests

TEST(Linq, WhereOdd)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);

    auto rng = from(src).where([](int a){return a%2 == 1;});

    for (int i = 1; i <= 5; i+=2)
    {
        EXPECT_EQ(i, rng.next());
    }

    EXPECT_THROW(rng.next(), LinqEndException);
}

TEST(Linq, WhereOdd_WhereLess)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto rng = from(src).where([](int a){return a%2 == 1;})
                        .where([](int a){return a < 4;});

    for (int i = 1; i <= 3; i+=2)
    {
        EXPECT_EQ(i, rng.next());
    }

    EXPECT_THROW(rng.next(), LinqEndException);
}

TEST(Linq, WhereLess_WhereOdd)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto rng = from(src).where([](int a){return a < 4;})
                        .where([](int a){return a%2 == 1;})
                        .toStdVector();

    std::vector<int> ans;
    ans.push_back(1);
    ans.push_back(3);

    EXPECT_EQ(ans,rng);
}

TEST(Linq, WhereLess_WhereOdd_OrderByDesc)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto rng = from(src).where([](int a){return a < 6;})
                        .where([](int a){return a%2 == 1;})
                        .orderBy([](int a){return -a;})
                        .toStdVector();

    std::vector<int> ans;
    ans.push_back(5);
    ans.push_back(3);
    ans.push_back(1);

    EXPECT_EQ(ans,rng);
}

TEST(Linq, WhereOdd_ToVector)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto dst = from(src).where([](int a){return a%2 == 1;})
                        .toStdVector();

    std::vector<int> ans;
    ans.push_back(1);
    ans.push_back(3);
    ans.push_back(5);
    ans.push_back(7);
    
    EXPECT_EQ(ans,dst);
}

TEST(Linq, WhereOdd_WhereLess_SelectMul2_ToVector)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto dst = from(src).where([](int a){return a%2 == 1;})
                        .where([](int a){return a < 6;})
                        .select([](int a){return a*2;})
                        .toStdVector();

    std::vector<int> ans;
    ans.push_back(2);
    ans.push_back(6);
    ans.push_back(10);

    EXPECT_EQ(ans,dst);
}

TEST(Linq, WhereOdd_WhereLess_SelectMul2_Reverse_ToVector)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto dst = from(src).where([](int a){return a%2 == 1;})
        .where([](int a){return a < 6;})
        .select([](int a){return a*2;})
        .reverse()
        .toStdVector();

    std::vector<int> ans;
    ans.push_back(10);
    ans.push_back(6);
    ans.push_back(2);

    EXPECT_EQ(ans,dst);
}

TEST(Linq, WhereOdd_Reverse_Reverse)
{
    std::vector<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);
    src.push_back(4);
    src.push_back(5);
    src.push_back(6);
    src.push_back(7);
    src.push_back(8);

    auto dst = from(src).where([](int a){return a%2 == 1;})
                        .reverse()
                        .where([](int a){return a < 4;})
                        .reverse()
                        .toStdVector();

    std::vector<int> ans;
    ans.push_back(1);
    ans.push_back(3);
    
    EXPECT_EQ(ans,dst);
}

//////////////////////////////////////////////////////////////////////////

TEST(Linq, Pointer_Front)
{
    int src[] = {1,2,3,4,5};

    auto dst = from(static_cast<int *>(src), static_cast<int *>(src) + 5);

    for(int i = 1; i <= 5; i++)
    {
        EXPECT_EQ(i, dst.next());
    }

    EXPECT_THROW(dst.next(), LinqEndException);
}


//////////////////////////////////////////////////////////////////////////

TEST(Linq, Array_Front)
{
    int src[] = {1,2,3,4,5};

    auto dst = from(src);

    for(int i = 1; i <= 5; i++)
    {
        EXPECT_EQ(i, dst.next());
    }

    EXPECT_THROW(dst.next(), LinqEndException);
}

//////////////////////////////////////////////////////////////////////////

TEST(Linq, Creations)
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    int arr[] = {1,2,3,4,5};
    //const int carr[] = {1,2,3,4,5};
    int * ptr = static_cast<int *>(arr);
    //const int * cptr = const_cast<int *>(arr);

    auto dst_vec = from(vec);
    auto dst_arr = from(arr);
    //auto dst_carr = from(carr);
    auto dst_ptr = from(ptr, ptr+5);
    //auto dst_cptr = from(cptr, cptr+5);
    auto dst_ptr_length = from(ptr, 5);
    //auto dst_cptr_length = from(cptr, 5);
    auto dst_vec_iter = from(vec.begin(), vec.end());
    //auto dst_vec_citer = from(vec.cbegin(), vec.cend());
}

//////////////////////////////////////////////////////////////////////////

TEST(Linq, MessagesCountUniqueContacts)
{
    struct Message
    {
        std::string PhoneA;
        std::string PhoneB;
        std::string Text;

        bool operator < (const Message & rhs) const
        {
            return (PhoneA < rhs.PhoneA)
                || ((PhoneA == rhs.PhoneA) && (PhoneB < rhs.PhoneB))
                || ((PhoneA == rhs.PhoneA) && (PhoneB == rhs.PhoneB) && (Text < rhs.Text));
        }
    };

    Message messages[] =
    {
        {"Anton","Denis","Hello, friend!"},
        {"Denis","Write","OLOLO"},
        {"Anton","Papay","WTF?"},
        {"Denis","Maloy","How r u?"},
        {"Denis","Write","Param-pareram!"},
    };

    int DenisUniqueContactCount = from(messages)
        .where(   [](const Message & msg){return msg.PhoneA == "Denis";})
        .distinct([](const Message & msg){return msg.PhoneB;})
        .count();

    EXPECT_EQ(2, DenisUniqueContactCount);
}

//////////////////////////////////////////////////////////////////////////

TEST(Linq, ForwardIterating)
{
    std::stringstream stream("0123456789");
    auto dst = from(std::istream_iterator<char>(stream),
                    std::istream_iterator<char>())
               .where( [](char a){return a % 2 == 0;})
               .select([](char a){return std::string(1,a);})
               .sum();

    EXPECT_EQ("02468", dst);
}
