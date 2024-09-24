#ifndef VPR_MOVE_GENERATOR_H
#define VPR_MOVE_GENERATOR_H

#include "vpr_types.h"
#include "move_utils.h"
#include "timing_place.h"
#include "directed_moves_util.h"

#include <limits>

class PlacerState;

struct MoveOutcomeStats {
    float delta_cost_norm = std::numeric_limits<float>::quiet_NaN();
    float delta_bb_cost_norm = std::numeric_limits<float>::quiet_NaN();
    float delta_timing_cost_norm = std::numeric_limits<float>::quiet_NaN();

    float delta_bb_cost_abs = std::numeric_limits<float>::quiet_NaN();
    float delta_timing_cost_abs = std::numeric_limits<float>::quiet_NaN();

    e_move_result outcome = ABORTED;
    float elapsed_time = std::numeric_limits<float>::quiet_NaN();
};

/**
 * @brief A Struct to hold statistics about the different move types
 *
 * blk_type_moves: the block type index of each proposed move (e.g. [0..NUM_PL_MOVE_TYPES][agent_available_types.size()-1)])
 * accepted_moves: the number of accepted moves of each move and block type (e.g. [0..NUM_PL_MOVE_TYPES][agent_available_types.size()-1)] )
 * rejected_moves: the number of rejected moves of each move and block type (e.g. [0..NUM_PL_MOVE_TYPES][agent_available_types.size()-1)] )
 *
 */
struct MoveTypeStat {
    vtr::NdMatrix<int, 2> blk_type_moves;
    vtr::NdMatrix<int, 2> accepted_moves;
    vtr::NdMatrix<int, 2> rejected_moves;

    /**
     * @brief Prints placement perturbation distribution by block and move type.
     */
    void print_placement_move_types_stats();
};

/**
 * @brief a base class for move generators
 *
 * This class represents the base class for all move generators.
 */
class MoveGenerator {
  public:

    /**
     * @brief Initializes some protected member variables that are used
     * by inheriting classes.
     *
     * @param placer_state A mutable reference to the placement state which will
     * be stored in this object.
     * @param reward_function Specifies the reward function to update q-tables
     * of the RL agent.
     */
    MoveGenerator(PlacerState& placer_state, e_reward_function reward_function)
        : placer_state_(placer_state)
        , reward_func_(reward_function) {}

    MoveGenerator() = delete;
    MoveGenerator(const MoveGenerator&) = delete;
    MoveGenerator& operator=(const MoveGenerator&) = delete;
    virtual ~MoveGenerator() = default;

    /**
     * @brief Updates affected_blocks with the proposed move, while respecting the current rlim
     *
     * This function proposes a new move and updates blocks affected and move_type accordingly. The function interface is general
     * to match the parameters needed by all move generators
     *
     *  @param blocks_affected: the output of the move
     *  @param proposed_action: Contains the move type and block type. If the block type is specified,
     *  the proposed move swaps instances of the given block type. Otherwise, the selected block type
     *  by the move generator is written to proposed_action.logical_blk_type_index.
     *  If proposed_action.logical_blk_type_index is -1, this function will choose the block from the netlist (regardless of type).
     *  @param rlim: maximum distance a block can move in x or y direction, in the compressed grid space
     *  @param placer_opts: all the placer options
     *  @param criticalities: the placer criticalities, useful for timing directed moves
     */
    virtual e_create_move propose_move(t_pl_blocks_to_be_moved& blocks_affected,
                                       t_propose_action& proposed_action,
                                       float rlim,
                                       const t_placer_opts& placer_opts,
                                       const PlacerCriticalities* criticalities) = 0;

    /**
     * @brief Recieves feedback about the outcome of the previously proposed move
     *
     * This function is very useful for RL agent to get the feedback to the agent
     *
     *  @param reward: the value of the agent's reward
     *  @param reward_fun: the name of the reward function used
     */
    virtual void process_outcome(double /*reward*/, e_reward_function /*reward_fun*/) {}

    /**
     * @brief Calculates the agent's reward and the total process outcome
     *
     * @param move_outcome_stats Contains information about how much each cost term
     * changes by this move and whether the move is accepted.
     * @param delta_c The total change in cost by this move.
     * @param timing_bb_factor This factor controls the weight of bb cost
     *  compared to the timing cost in the agent's reward function.
     */
    void calculate_reward_and_process_outcome(const MoveOutcomeStats& move_outcome_stats,
                                              double delta_c,
                                              float timing_bb_factor);

  protected:
    std::reference_wrapper<PlacerState> placer_state_;
    e_reward_function reward_func_;
};

#endif
