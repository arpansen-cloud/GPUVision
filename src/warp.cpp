#include "warp.h"

#include <stdexcept>
#include <utility>

namespace gpuvision {

bool Instruction::operator==(const Instruction& other) const {
    return type == other.type;
}

bool Instruction::operator!=(const Instruction& other) const {
    return !(*this == other);
}

Warp::Warp(int id, std::vector<Instruction> instructions)
    : id_(id),
      program_counter_(0),
      state_(instructions.empty() ? WarpState::COMPLETED : WarpState::READY),
      stall_cycles_remaining_(0),
      instructions_(std::move(instructions)) {}

int Warp::id() const {
    return id_;
}

std::size_t Warp::program_counter() const {
    return program_counter_;
}

WarpState Warp::state() const {
    return state_;
}

int Warp::stall_cycles_remaining() const {
    return stall_cycles_remaining_;
}

std::size_t Warp::instruction_count() const {
    return instructions_.size();
}

const std::vector<Instruction>& Warp::instructions() const {
    return instructions_;
}

bool Warp::is_ready() const {
    return state_ == WarpState::READY;
}

bool Warp::is_completed() const {
    return state_ == WarpState::COMPLETED;
}

void Warp::tick_stall() {
    if (state_ != WarpState::STALLED) {
        return;
    }

    if (stall_cycles_remaining_ > 0) {
        --stall_cycles_remaining_;
    }

    if (stall_cycles_remaining_ == 0) {
        state_ = program_counter_ >= instructions_.size() ? WarpState::COMPLETED : WarpState::READY;
    }
}

bool Warp::issue_next_instruction(int memory_latency) {
    if (state_ != WarpState::RUNNING && state_ != WarpState::READY) {
        throw std::logic_error("Cannot issue a warp that is not ready or running");
    }

    if (program_counter_ >= instructions_.size()) {
        state_ = WarpState::COMPLETED;
        return false;
    }

    const Instruction instruction = instructions_[program_counter_];
    ++program_counter_;

    if (instruction.type == InstructionType::MEMORY) {
        stall_cycles_remaining_ = memory_latency;
        state_ = stall_cycles_remaining_ > 0 ? WarpState::STALLED : WarpState::READY;
        if (program_counter_ >= instructions_.size() && stall_cycles_remaining_ == 0) {
            state_ = WarpState::COMPLETED;
        }
        return true;
    }

    state_ = program_counter_ >= instructions_.size() ? WarpState::COMPLETED : WarpState::READY;
    return false;
}

void Warp::mark_running() {
    if (state_ == WarpState::READY) {
        state_ = WarpState::RUNNING;
    }
}

void Warp::mark_ready() {
    if (state_ == WarpState::RUNNING) {
        state_ = WarpState::READY;
    }
}

std::string to_string(InstructionType type) {
    switch (type) {
        case InstructionType::COMPUTE:
            return "COMPUTE";
        case InstructionType::MEMORY:
            return "MEMORY";
    }
    return "UNKNOWN";
}

std::string to_string(WarpState state) {
    switch (state) {
        case WarpState::READY:
            return "READY";
        case WarpState::RUNNING:
            return "RUNNING";
        case WarpState::STALLED:
            return "STALLED";
        case WarpState::COMPLETED:
            return "COMPLETED";
    }
    return "UNKNOWN";
}

} // namespace gpuvision
