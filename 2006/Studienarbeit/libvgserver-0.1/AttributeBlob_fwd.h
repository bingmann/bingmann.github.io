// $Id: AttributeBlob_fwd.h 47 2006-01-26 19:08:04Z bingmann $

#ifndef VGS_AttributeBlob_fwd_H
#define VGS_AttributeBlob_fwd_H

namespace VGServer {

template <typename AllocPolicy>
class TpAttributeBlob;

typedef TpAttributeBlob<class AttributeBigBlobPolicy> AttributeBigBlob;

typedef TpAttributeBlob<class AttributeTinyBlobPolicy> AttributeTinyBlob;

} // namespace VGServer

#endif // VGS_AttributeBlob_fwd_H
