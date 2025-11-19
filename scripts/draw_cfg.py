#!/usr/bin/env python3
# -- coding: utf-8 --

from typing import List
import graphviz as gv
import sys
import os
import argparse


class BasicBlock:
    def __init__(self, ident: int) -> None:
        self.id = ident
        self.insts = []
        self.preds = []
        self.succs = []
        self.props = []

    def text(self, no_insts=False) -> str:
        s = 'BB ' + str(self.id) + '\n'
        s += 'props: '
        for i, prop in enumerate(self.props):
            if i > 0:
                s += ', '
            s += prop
        s += '\n'

        if not no_insts:
            for inst in self.insts:
                s += inst + '\n'
        return s


class Function:
    def __init__(self, method: str, blocks: List[BasicBlock]) -> None:
        self.method = method
        self.blocks = blocks


class GraphDumpParser:
    def __init__(self) -> None:
        self.method = None
        self.block = None
        self.blocks = []
        self.functions = []

    def finish_block(self):
        if self.block:
            self.blocks.append(self.block)
            self.block = None

    def finish_function(self):
        self.finish_block()
        if self.method:
            self.functions.append(Function(self.method, self.blocks))
            self.blocks = []

    def begin_block(self, line: str):
        self.finish_block()

        preds_start = line.find('preds:')
        block_id = int(line[len('BB'):preds_start].strip())
        self.block = BasicBlock(block_id)
        if preds_start != -1:
            self.parse_block_preds(line, preds_start)

    def begin_function(self, line: str):
        self.finish_function()
        self.method = line[len('method:'):].strip()

    def parse_block_preds(self, line: str, preds_start: int):
        preds = line[preds_start+len('preds:'):].strip().strip('[]')
        for pred in preds.split(','):
            pred = pred.strip()
            self.block.preds.append(int(pred[len('bb'):].strip()))

    def parse_block_props(self, line: str):
        for s in line[len('prop:'):].strip().split(','):
            s = s.strip()
            if not s.startswith('bc'):
                self.block.props.append(s)

    def parse_block_succs(self, line: str):
        succs = line[len('succs:'):].strip().strip('[]')
        for succ in succs.split(','):
            succ = succ.strip()
            self.block.succs.append(int(succ[len('bb'):].strip()))
        self.finish_block()

    def parse(self, lines: List[str]) -> List[Function]:
        for line in lines:
            if line.startswith('Method:'):
                self.begin_function(line)
            elif line.startswith('BB'):
                self.begin_block(line)
            elif self.block:
                if line.startswith('prop:'):
                    self.parse_block_props(line)
                elif line.startswith('succs:'):
                    self.parse_block_succs(line)
                else:
                    self.block.insts.append(line)
        self.finish_function()
        return self.functions


def draw_function(function: Function, out_dir=None, no_insts=False):
    dot = gv.Digraph(format='png')
    for block in function.blocks:
        dot.node(str(block.id), block.text(no_insts), shape='box')
    for block in function.blocks:
        for succ in block.succs:
            dot.edge(str(block.id), str(succ))
    basename = 'cfg_' + function.method
    dotfile_path = os.path.join(out_dir, basename)
    dot.render(basename, out_dir, format="png")
    os.rename(dotfile_path, dotfile_path + '.dot')


def main():
    parser = argparse.ArgumentParser(description="A tool for drawing CFGs by reading ir dump from stdin")
    parser.add_argument("-i", "--input", type=str, default="input.txt", help="the file containing the IR dump (default: 'input.txt')")
    parser.add_argument("--no-insts", action="store_true", help="drawing without IR instructions")
    parser.add_argument("-o", "--output", type=str, default="./out", help="output directory, default to './out'")
    args = parser.parse_args()

    with open(args.input, 'r') as file:
        lines = file.readlines()

    functions = GraphDumpParser().parse(lines)
    for function in functions:
        draw_function(function, args.output, no_insts=args.no_insts)


if __name__ == "__main__":
    main()
