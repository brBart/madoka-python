# -*- coding: utf-8 -*-
from nose.tools import assert_equal, assert_true
import madoka
import os


class Test_madoka(object):

    def test___setitem__(self):
        sketch = madoka.Sketch()
        sketch['mami'] += 20
        assert_equal(sketch.get('mami'), 20)
        sketch['mami'] *= 2
        assert_equal(sketch.get('mami'), 40)

    def test___getitem__(self):
        sketch = madoka.Sketch()
        sketch.inc('mami')
        assert_equal(sketch['mami'], 1)

    def test_inc(self):
        sketch = madoka.Sketch()
        sketch.inc('mami')
        assert_equal(sketch['mami'], 1)
        sketch.inc('mami')
        assert_equal(sketch['mami'], 2)

    def test_add(self):
        sketch = madoka.Sketch()
        sketch.add('mami', 2)
        assert_equal(sketch['mami'], 2)
        sketch.add('mami', 8)
        assert_equal(sketch['mami'], 10)
        sketch['mami'] += 10
        assert_equal(sketch['mami'], 20)

    def test_set(self):
        sketch = madoka.Sketch()
        sketch.set('mami', 14)
        assert_equal(sketch['mami'], 14)

    def test_clear(self):
        sketch = madoka.Sketch()
        sketch['mami'] = 14
        sketch.clear()
        assert_equal(sketch['mami'], 0)

    def test_create(self):
        sketch = madoka.Sketch()
        sketch.create(max_value=4)
        sketch['mami'] = 100
        assert_equal(sketch['mami'], 15)

    def test_copy(self):
        sketch = madoka.Sketch()
        sketch['mami'] = 14

        new_sketch = madoka.Sketch()
        new_sketch.copy(sketch)
        assert_equal(new_sketch['mami'], 14)

    def test_merge(self):
        sketch = madoka.Sketch()
        sketch['mami'] = 14

        new_sketch = madoka.Sketch()
        new_sketch['mami'] = 14

        new_sketch.merge(sketch)
        assert_equal(new_sketch['mami'], 28)

    def test_inner_product(self):
        sketch = madoka.Sketch()
        sketch['mami'] = 2
        sketch['homura'] = 1
        sketch['kyouko'] = 2
        sketch['sayaka'] = 2

        new_sketch = madoka.Sketch()
        new_sketch['mami'] = 2
        new_sketch['kyouko'] = 3
        new_sketch['sayaka'] = 10

        assert_equal(new_sketch.inner_product(sketch), 30)

    def test_save_and_load(self):
        try:
            filename = 'test.madoka'
            sketch = madoka.Sketch()
            sketch['mami'] = 14
            sketch.save(filename)
            assert_true(os.path.exists(filename))

            sketch = madoka.Sketch()
            sketch.load(filename)
            assert_equal(sketch['mami'], 14)
            sketch = madoka.Sketch()
            sketch.open(filename)
            assert_equal(sketch['mami'], 14)
        finally:
            os.remove(filename)
