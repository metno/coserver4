
#include "CoServer4.h"

#include "TestHelpers.h"

#include <QUrl>

#include <gtest/gtest.h>

namespace {
const QString ALF("Alf"), BERT("Bert");
const QString TYPE1("type1"), TYPE2("type2"), TYPEBC("watcher");

class Co4Test3 : public ClientFixture {
public:
    Co4Test3();

    CoClient_x ca1, ca2, cb1, cb2, cbc;
    MessageReceiver_x mca1, mca2, mcb1, mcb2, mcbc;
};

Co4Test3::Co4Test3()
    : ClientFixture("local:///" TEST_BUILDDIR "test3.sock")
{
    ca1 = addClient(mca1, TYPE1, ALF);
    ca2 = addClient(mca2, TYPE2, ALF);
    cb1 = addClient(mcb1, TYPE1, BERT);
    cb2 = addClient(mcb2, TYPE2, BERT);
    cbc = addClient(mcbc, TYPEBC, "");
    clearCounts();
}

} // namespace


TEST_F(Co4Test3, Broadcast)
{
    miQMessage msg("broadcast");
    cbc->sendMessage(msg);
    test_wait();

    EXPECT_EQ(1, mca1->count());
    EXPECT_EQ(1, mca2->count());
    EXPECT_EQ(1, mcb1->count());
    EXPECT_EQ(1, mcb2->count());
    EXPECT_EQ(0, mcbc->count());
}

TEST_F(Co4Test3, AToA)
{
    miQMessage msg("from A to A");
    ca1->sendMessage(msg);
    test_wait();

    EXPECT_EQ(0, mca1->count());
    EXPECT_EQ(1, mca2->count());
    EXPECT_EQ(0, mcb1->count());
    EXPECT_EQ(0, mcb2->count());
    EXPECT_EQ(1, mcbc->count());
}

TEST_F(Co4Test3, A1ToA2)
{
    miQMessage msg("from A1 to A2");
    ca1->sendMessage(msg, clientId(ca2->getClientId()));
    test_wait();

    EXPECT_EQ(0, mca1->count());
    EXPECT_EQ(1, mca2->count());
    EXPECT_EQ(0, mcb1->count());
    EXPECT_EQ(0, mcb2->count());
    EXPECT_EQ(0, mcbc->count());
}

TEST_F(Co4Test3, BCToA1B2)
{
    miQMessage msg("from BC to A1, B2");
    ClientIds ids;
    ids.insert(ca1->getClientId());
    ids.insert(cb2->getClientId());
    ASSERT_EQ(2, ids.size());
    cbc->sendMessage(msg, ids);
    test_wait();

    EXPECT_EQ(1, mca1->count());
    EXPECT_EQ(0, mca2->count());
    EXPECT_EQ(0, mcb1->count());
    EXPECT_EQ(1, mcb2->count());
    EXPECT_EQ(0, mcbc->count());
}
