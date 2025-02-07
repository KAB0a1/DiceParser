#include "explodedicenode.h"
#include "diceparser/parsingtoolbox.h"
#include "validatorlist.h"

ExplodeDiceNode::ExplodeDiceNode() : m_diceResult(new DiceResult())
{
    m_result= m_diceResult;
}
void ExplodeDiceNode::run(ExecutionNode* previous)
{
    m_previousNode= previous;
    if(!previous)
        return;

    if(!previous->getResult())
        return;

    DiceResult* previous_result= dynamic_cast<DiceResult*>(previous->getResult());
    m_result->setPrevious(previous_result);

    if(!previous_result)
        return;

    for(auto& die : previous_result->getResultList())
    {
        Die* tmpdie= new Die(*die);
        m_diceResult->insertResult(tmpdie);
        die->displayed();
    }

    quint64 limit= -1;
    if(m_limit)
    {
        m_limit->run(this);
        auto limitNode= ParsingToolBox::getLeafNode(m_limit);
        auto result= limitNode->getResult();
        if(result->hasResultOfType(Dice::RESULT_TYPE::SCALAR))
            limit= static_cast<quint64>(result->getResult(Dice::RESULT_TYPE::SCALAR).toInt());
    }

    bool hasExploded= false;
    std::function<void(Die*, qint64)> f= [&hasExploded, this, limit](Die* die, qint64) {
        static QHash<Die*, quint64> explodePerDice;
        if(Dice::CONDITION_STATE::ALWAYSTRUE
           == m_validatorList->isValidRangeSize(std::make_pair<qint64, qint64>(die->getBase(), die->getMaxValue())))
        {
            m_errors.insert(Dice::ERROR_CODE::ENDLESS_LOOP_ERROR,
                            QObject::tr("Condition (%1) cause an endless loop with this dice: %2")
                                .arg(toString(true))
                                .arg(QStringLiteral("d[%1,%2]")
                                         .arg(static_cast<int>(die->getBase()))
                                         .arg(static_cast<int>(die->getMaxValue()))));
        }
        hasExploded= true;
        if(limit >= 0)
        {
            auto& d= explodePerDice[die];
            if(d == limit)
            {
                hasExploded= false;
                return;
            }
            ++d;
        }
        die->roll(true);
    };
    do
    {
        hasExploded= false;
        m_validatorList->validResult(m_diceResult, false, false, f);
    } while(hasExploded);

    if(nullptr != m_nextNode)
    {
        m_nextNode->run(this);
    }
}
ExplodeDiceNode::~ExplodeDiceNode()
{
    if(nullptr != m_validatorList)
    {
        delete m_validatorList;
    }
}
void ExplodeDiceNode::setValidatorList(ValidatorList* val)
{
    m_validatorList= val;
}
QString ExplodeDiceNode::toString(bool withlabel) const
{
    if(withlabel)
    {
        return QString("%1 [label=\"ExplodeDiceNode %2\"]").arg(m_id, m_validatorList->toString());
    }
    else
    {
        return m_id;
    }
}
qint64 ExplodeDiceNode::getPriority() const
{
    qint64 priority= 0;
    if(nullptr != m_previousNode)
    {
        priority= m_previousNode->getPriority();
    }
    return priority;
}

ExecutionNode* ExplodeDiceNode::getCopy() const
{
    ExplodeDiceNode* node= new ExplodeDiceNode();
    if(nullptr != m_validatorList)
    {
        node->setValidatorList(m_validatorList->getCopy());
    }
    if(nullptr != m_nextNode)
    {
        node->setNextNode(m_nextNode->getCopy());
    }
    return node;
}

void ExplodeDiceNode::setLimitNode(ExecutionNode* limitNode)
{
    m_limit= limitNode;
}
