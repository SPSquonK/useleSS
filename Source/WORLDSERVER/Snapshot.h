#pragma once

#include "Ar.h"
#include "mempooler.h"
#include <DPlay.h>

// @SPSquonK, 2019-08~2022-05
// Improve CSnapshot class with size of snapshots
// - This lets the client only parse partially the received snapshots
// and detect more easily programming errors.
// - Released under the Boost License

class CSnapshot final {
public:
	// Proxy class to provide backward compatibility with the
	// old m_Snapshot.cb++ instructions.
	struct Cb {
		friend CSnapshot;
		CSnapshot * m_self;

		explicit Cb(CSnapshot * self) : m_self(self) {}
		
		// The pointer to self makes it uncopyable
		Cb(const Cb &) = delete;
		Cb & operator=(const Cb &) = delete;
		~Cb() = default;

		void operator++(int) { m_self->OnNewSnapshot(); }
	};

public:
	DPID	dpidCache;
	DPID	dpidUser;
	CAr		ar;
	Cb    cb;

private:
	short	nbOfSnapshots = 0;
	u_long previousOffset = 0;
public:
	// Constructions
	CSnapshot();
	CSnapshot( DPID idCache, DPID idUser, OBJID objid, DWORD dwHdr );
	CSnapshot(const CSnapshot &) = delete;
	CSnapshot & operator=(const CSnapshot &) = delete;
	~CSnapshot() = default;

	// Operations
	void	SetSnapshot( OBJID objid, DWORD dwHdr );
	void	Flush() {
		dpidCache = dpidUser = DPID_UNKNOWN;
		ar.Flush();
		nbOfSnapshots = 0;
		previousOffset = 0;
	}

	// Declares a new snapshot, registering the size of the previous one if any
	void OnNewSnapshot() {
		if (nbOfSnapshots != 0) {
			FinalizeSnapshot();
		}

		++nbOfSnapshots;
		previousOffset = ar.GetOffset();
		ar << u_long(0);
	}

	// Returns the number of ducks encountered since the player connection
	[[nodiscard]] WORD GetNumberOfSnapshots() const {
		// TODO: maybe check if nbOfSnapshots < std::numeric_limits<WORD>::max?
		return static_cast<WORD>(nbOfSnapshots);
	}

	// Considering this is the end of the currently written snapshot,
	// write the size of the snapshot at the beginning
	void FinalizeSnapshot() {
		const u_long current = ar.GetOffset();
		const u_long size = current - previousOffset;

		int _;
		BYTE * buffer = ar.GetBuffer(&_);
		*(UNALIGNED u_long *)(buffer + previousOffset) = size;
	}

	// Finalizes the current snapshot and return true if there are any
	// snapshot to send.
	bool PrepareSend() {
		if (nbOfSnapshots == 0) return false;
		FinalizeSnapshot();
		return true;
	}

	// Clears the list of snapshots
	void Reset(u_long reelTowards) {
		nbOfSnapshots = 0;
		ar.ReelIn(reelTowards);
	}
};
