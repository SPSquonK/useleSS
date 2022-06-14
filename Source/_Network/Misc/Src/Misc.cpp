#include "StdAfx.h"
#include "ar.h"
#include "Misc.h"

CAr & operator<<(CAr & ar, const SERVER_DESC & self) {
  ar << self.dwParent;
  ar << self.dwID;
  ar.WriteString(self.lpName);
  ar.WriteString(self.lpAddr);
  ar << self.b18;
  ar << self.lCount;
  ar << self.lEnable;
  ar << self.lMax;
  return ar;
}

CAr & operator>>(CAr & ar, SERVER_DESC & self) {
  ar >> self.dwParent;
  ar >> self.dwID;
  ar.ReadString(self.lpName);
  ar.ReadString(self.lpAddr);
  ar >> self.b18;
  ar >> self.lCount;
  ar >> self.lEnable;
  ar >> self.lMax;
  return ar;
}
