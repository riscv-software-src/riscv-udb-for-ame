#pragma once

#include "udb/bits.hpp"
#include "udb/bitfield.hpp"
#include "udb/pool_alloc.hpp"
#include "udb/reg.hxx"

namespace udb {

  class InstBase {
   public:
    InstBase() = default;
    ~InstBase() = default;

    virtual uint64_t pc() const = 0;
    virtual uint64_t encoding() const = 0;

    // return encoding length, in bytes
    virtual size_t enc_len() const = 0;

    // execute the instruction, updating the hart state
    // if the instruction causes a synchronous exception, a
    // HartBase::SynchronousException will be raised in C++
    virtual void execute() = 0;

    virtual const std::string_view &name() = 0;
    virtual std::string disassemble(bool use_abi_reg_names = false) const = 0;

    // true if the instruction could change the pc without causing an exception
    // i.e., is a branch
    virtual bool control_flow() const = 0;

    // return the
    virtual std::vector<Reg> srcRegs() const = 0;
    virtual std::vector<Reg> dstRegs() const = 0;

  };

  template <unsigned XLEN, unsigned EncodingLength>
    requires (EncodingLength % 8 == 0)
  class InstWithKnownLength : public InstBase {
   public:
    using EncodingType = Bits<EncodingLength>;

    InstWithKnownLength(Bits<XLEN> pc, EncodingType encoding)
      : m_pc(pc),
        m_encoding(encoding)
    {
    }

    uint64_t pc() const override { return m_pc.get(); }
    const Bits<XLEN> _pc() const { return m_pc; }
    uint64_t encoding() const override { return m_encoding.get(); }
    const EncodingType& _encoding() const { return m_encoding; }
    size_t enc_len() const override { return EncodingLength / 8; }

   protected:
    const Bits<XLEN> m_pc;
    const EncodingType m_encoding;
  };

}  // namespace udb
