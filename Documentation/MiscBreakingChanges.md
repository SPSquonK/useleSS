# Misc breaking changes


## CoreServer.ini

- To add a world map, you only have to write its world id. Do not add the
other usual numbers.

- You may write the map define instead of its ID

*Other servers*
```
0101
{
	1	0	0	30	30	00	00
	2	0	0	5	5	00	00
	3	0	0	5	5	00	00
	21	0	0	1	1	00	00
	22	0	0	1	1	00	00
	23	0	0	1	1	00	00
}
```

*useleSS*
```
0101
{
    WI_WORLD_MADRIGAL
    WI_WORLD_KEBARAS
    WI_WORLD_CISLAND
    21 /* WI_WORLD_HEAVEN01 */
    22 /* WI_WORLD_HEAVEN02 */
    23 /* WI_WORLD_HEAVEN03 */
}
```


