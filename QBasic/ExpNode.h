# pragma once

#include <iostream>
#include <qstring.h>
#include <string>

enum ExpNodeType {
    exp_var,
    exp_num,
    exp_op,
    exp_neg,
    exp_ins
};

enum ExpPartType {
    exp_var_part,
    exp_num_part,
    exp_add_part,
    exp_sub_part,
    exp_mul_part,
    exp_div_part,
    exp_lbracket_part,
    exp_rbracket_part,
    exp_greater_part,
    exp_equal_part,
    exp_less_part,
    exp_mod_part,
    exp_pow_part,
    exp_neg_part,
    exp_rem_part,
    exp_let_part,
    exp_print_part,
    exp_input_part,
    exp_goto_part,
    exp_if_part,
    exp_end_part,
    exp_error_part,
    exp_empty_part,
    exp_ins_part
};

class ExpNode
{
  private:
    /* data */
  public:
    ExpNodeType type;
    std::string exp;
    ExpNode *child;
    ExpNode *next;
    ExpNode *parent;
    ExpNode(/* args */){};
    ExpNode(ExpNodeType type, std::string exp, ExpNode *parent,ExpNode *child, ExpNode *next) : type(type), parent(parent), exp(exp), child(child), next(next){
        // 删除exp中的空格
        this->exp.erase(std::remove(exp.begin(), exp.end(), ' '), exp.end());
        if (child != NULL)
            child->parent = this;
        if (next != NULL)
            next->parent = this;

    };
    ExpNode(ExpNodeType type, std::string exp, ExpNode *parent) : type(type), exp(exp), child(NULL), next(NULL), parent(parent){
      if(exp != "")
      this->exp.erase(std::remove(this->exp.begin(), this->exp.end(), ' '), this->exp.end());
    };
    ~ExpNode(){};
};
