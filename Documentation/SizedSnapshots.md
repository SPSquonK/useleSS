# Sized World to Client Snapshots

*This is essentially the v2 of https://gist.github.com/SPSquonK/cb9f51633f2758e1fc814a7f7e768173*


In FlyfF, some packets send multiple message: that's the snapshot system.

## Why

One issue with the basic implementation is that the developer is forced to
consume all bytes of a snapshot, and it is impossible to catch an error at
run time. If a snapshot do not consume all bytes, and only the intended bytes,
further snapshots will be glitched.

That's bad because:
- You can not return early in snapshot handler functions because you have
to process the data. This is done in the official source by often having
to fill some to be discarded structures, like inventories.
    - Returning early in cases of error is important for readability
- It makes some bugs hard to catch: an error that happens in a handler
can come from another handler. It can even lead to incoherent data in the
client memory, leading to crashes that occurs in totally different places.


## What

The solution is to add the size of each snapshot at the beginning of them.

When we handle a snapshot, we register the size, and in all cases, after the
processing we go at the expected end of the snapshot.


The exact code change can be seen in [commit bd3d1aec5d0d4ca512ce89a8f0f2c290c3eb6e40](https://github.com/SPSquonK/useleSS/commit/bd3d1aec5d0d4ca512ce89a8f0f2c290c3eb6e40).


## Why is this system better than your old code.

The major different is that we provide backward compatibility with old
"CUser::Add..." functions. In other words, you do not have to change
the code of such functions: 

```cpp
void CUser::AddReplace( DWORD dwWorldID, D3DXVECTOR3 & vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REPLACE;
	m_Snapshot.ar << dwWorldID;
	m_Snapshot.ar << vPos;
}
```

Is still compatible with the system.

How we achieve that is by replacing the class of Cb, from short to a struct
that will call `OnNewSnapshot` to the parent CSnapshot class.

This is good because:
- You do not have to change the functions of your source code nor your habits
- If you are selling systems, you can continue to write m_Snapshot.cb++, and
if will continue to be compatible with the code of people that did change
nothing to CSnapshot behavior or made backward compatible changes.


