#ifndef GPUVISION_WARP_H
#define GPUVISION_WARP_H

#include <cstddef>
#include <string>
#include <vector>

namespace gpuvision {

// A warp is a group of GPU threads that execute one instruction stream together.
enum class InstructionType {
    COMPUTE,
    MEMORY
};

struct Instruction {
    InstructionType type;

    bool operator==(const Instruction& other) const;
    bool operator!=(const Instruction& other) const;
};

enum class WarpState {
    READY,
    RUNNING,
    STALLED,
    COMPLETED
};

class Warp {
public:
    Warp(int id, std::vector<Instruction> instructions);

    int id() const;
    std::size_t program_counter() const;
    WarpState state() const;
    int stall_cycles_remaining() const;
    std::size_t instruction_count() const;
    const std::vector<Instruction>& instructions() const;

    bool is_ready() const;
    bool is_completed() const;

    // Advances a stalled warp by one cycle. When the stall expires, the warp is ready.
    void tick_stall();

    // Issues exactly one instruction for this warp.
    // Returns true when the instruction was a memory operation.
    bool issue_next_instruction(int memory_latency);

    void mark_running();
    void mark_ready();

private:
    int id_;
    std::size_t program_counter_;
    WarpState state_;
    int stall_cycles_remaining_;
    std::vector<Instruction> instructions_;
};

std::string to_string(InstructionType type);
std::string to_string(WarpState state);

} // namespace gpuvision

#endif // GPUVISION_WARP_H
