<p align="center"><img alt="title" src="https://hcti.io/v1/image/34cca40e-bd4b-44ec-ad18-29f7656c4c1e"></p>
<p align="center"><img alt="GitHub" src="https://img.shields.io/github/license/Hyiker/tiny_rasterizer?style=flat-square"></p>

## Introduction

This is my own rasterizer implementation. Developed in C++17.

## Usage

1. compile
2. `./rasterizer [dist] [model] [texture(optional)]`

## Roadmap

- [x] basic rendering pipeline
- [x] texture support
- [x] bling-phong light
- [ ] shadow
- [ ] shader language support(Optional)

## Dependencies

- [LodePNG](https://github.com/lvandeve/lodepng) (Header Only)
- [OBJ Loader](https://github.com/Bly7/OBJ-Loader) (Header Only)

## Example

![Cow](./images/cow_text.png)

Cow rendered using texture shader.

## Reference

- [GAMES101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)
