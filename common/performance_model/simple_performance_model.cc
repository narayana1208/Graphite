#include "performance_model.h"
#include "log.h"

using std::endl;

SimplePerformanceModel::SimplePerformanceModel()
    : m_instruction_count(0)
    , m_cycle_count(0)
{
}

SimplePerformanceModel::~SimplePerformanceModel()
{
}

void SimplePerformanceModel::outputSummary(std::ostream &os)
{
   os << "  Instructions: " << getInstructionCount() << endl
      << "  Cycles: " << getCycleCount() << endl;
}

void SimplePerformanceModel::handleInstruction(Instruction *instruction)
{
   // compute cost
   UInt64 cost = 0;

   const OperandList &ops = instruction->getOperands();
   for (unsigned int i = 0; i < ops.size(); i++)
   {
      const Operand &o = ops[i];

      if (o.m_type == Operand::MEMORY)
      {
         DynamicInstructionInfo &info = getDynamicInstructionInfo();

         if (o.m_direction == Operand::READ)
         {
            LOG_ASSERT_ERROR(info.type == DynamicInstructionInfo::MEMORY_READ,
                             "Expected memory read info, got: %d.", info.type);

            cost += info.memory_info.latency;
            // ignore address
         }
         else
         {
            LOG_ASSERT_ERROR(info.type == DynamicInstructionInfo::MEMORY_WRITE,
                             "Expected memory write info, got: %d.", info.type);
         }

         popDynamicInstructionInfo();
      }
   }

   cost += instruction->getCost();
   LOG_ASSERT_WARNING(cost < 10000, "Cost is too big - cost:%llu, cycle_count: %llu, type: %d", cost, m_cycle_count, instruction->getType());

   // update counters
   m_instruction_count++;
   m_cycle_count += cost;
}

