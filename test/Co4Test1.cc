
#include "TestHelpers.h"

namespace {
const QString ALF("Alf");
const QString TYPE1("type1"), TYPE2("type2");

class Co4Test1 : public ClientFixture {
public:
    Co4Test1();
    CoClient_x c1, c2;
    MessageReceiver_x mc1, mc2;
};

Co4Test1::Co4Test1()
    : ClientFixture("local:///" TEST_BUILDDIR "test1.sock")
{
    c1 = addClient(mc1, TYPE1, ALF);
    c2 = addClient(mc2, TYPE2, ALF);
    clearCounts();
}

} // namespace

TEST_F(Co4Test1, TwoClients)
{
    EXPECT_LT(0, c1->getClientId());
    EXPECT_LT(0, c2->getClientId());
    EXPECT_NE(c1->getClientId(), c2->getClientId());
}
