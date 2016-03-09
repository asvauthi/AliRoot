//-*- Mode: C++ -*-
#ifndef ALIHLTTPCDATACOMPRESSIONUNPACKERCOMPONENT_H
#define ALIHLTTPCDATACOMPRESSIONUNPACKERCOMPONENT_H
//* This file is property of and copyright by the ALICE HLT Project        *
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/// @file   AliHLTTPCDataCompressionUnpackerComponent.h
/// @author Matthias Richter
/// @date   2016-03-08
/// @brief  Unpacker component for compressed TPC cluster data
///

#include "AliHLTProcessor.h"
#include "AliHLTTPCRawCluster.h"
#include "AliHLTTPCSpacePointData.h"
#include "TString.h"
#include <map>
#include <cassert>

class AliHLTTPCClusterMCLabel;
class AliHLTTPCDataCompressionDecoder;

/**
 * @class AliHLTTPCDataCompressionUnpackerComponent
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b TPCDataCompressorUnpacker      <br>
 * Library: \b libAliHLTTPC.so     <br>
 * Input Data Types:  <br>
 * Output Data Types: <br>
 *
 * <h2>Mandatory arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * <h2>Optional arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->

 * <h2>Configuration:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * <h2>Default CDB entries:</h2>
 *
 * <h2>Performance:</h2>
 *
 * <h2>Memory consumption:</h2>
 *
 * <h2>Output size:</h2>
 *
 *
 * @ingroup alihlt_tpc
 */
class AliHLTTPCDataCompressionUnpackerComponent : public AliHLTProcessor {
public:
  /// standard constructor
  AliHLTTPCDataCompressionUnpackerComponent();
  /// destructor
  ~AliHLTTPCDataCompressionUnpackerComponent();
  /// inherited from AliHLTComponent: id of the component
  virtual const char* GetComponentID();

  /// inherited from AliHLTComponent: list of data types in the vector reference
  void GetInputDataTypes( AliHLTComponentDataTypeList& );

  /// inherited from AliHLTComponent: output data type of the component.
  AliHLTComponentDataType GetOutputDataType();

  /// inherited from AliHLTComponent: multiple output data types of the component.
  int GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList);

  /// inherited from AliHLTComponent: output data size estimator
  void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );

  /// inherited from AliHLTComponent: spawn function.
  virtual AliHLTComponent* Spawn();

  /**
   * @class AliClusterWriter
   * Container for cluster decoder.
   * The class implements the interface to be used in the decoding
   * of compressed TPC data, and writes it to the output buffer.
   */
  class AliClusterWriter : public AliHLTLogging {
  public:
    AliClusterWriter();
    AliClusterWriter(AliHLTUInt8_t* pBuffer, AliHLTUInt32_t size);
    virtual ~AliClusterWriter();

    struct AliClusterIdBlock {
      AliClusterIdBlock() : fIds(NULL), fSize(0) {}
      AliHLTUInt32_t* fIds; //!
      AliHLTUInt32_t  fSize; //!
    };

    class iterator {
    public:
      iterator() : fClusterNo(-1), fData(NULL), fSlice(-1), fPartition(-1), fClusterId(kAliHLTVoidDataSpec) {}
      iterator(AliClusterWriter* pData, int slice=-1, int partition=-1) : fClusterNo(-1), fData(pData), fSlice(slice), fPartition(partition), fClusterId(GetClusterId()) {}
      iterator(const iterator& other) : fClusterNo(other.fClusterNo), fData(other.fData), fSlice(other.fSlice), fPartition(other.fPartition), fClusterId(other.fClusterId) {}
      iterator& operator=(const iterator& other) {
        if (this==&other) return *this;
        fClusterNo=other.fClusterNo; fData=other.fData; fSlice=other.fSlice; fPartition=other.fPartition; fClusterId=other.fClusterId; return *this;
      }
      ~iterator() {}

      void SetPadRow(int row)             {if (fData) fData->GetClusterRef(fClusterId).SetPadRow(row);}
      void SetPad(float pad)              {if (fData) fData->GetClusterRef(fClusterId).SetPad(pad);}
      void SetTime(float time)            {if (fData) fData->GetClusterRef(fClusterId).SetTime(time);}
      void SetSigmaY2(float sigmaY2)      {if (fData) fData->GetClusterRef(fClusterId).SetSigmaPad2(sigmaY2);}
      void SetSigmaZ2(float sigmaZ2)      {if (fData) fData->GetClusterRef(fClusterId).SetSigmaTime2(sigmaZ2);}
      void SetCharge(unsigned charge)     {if (fData) fData->GetClusterRef(fClusterId).SetCharge(charge);}
      void SetQMax(unsigned qmax)         {if (fData) fData->GetClusterRef(fClusterId).SetQMax(qmax);}
      void Set(const AliHLTTPCRawCluster& cl) {*this=cl;}
      iterator& operator=(const AliHLTTPCRawCluster& cluster) {if (fData) {
          fData->GetClusterRef(fClusterId)=cluster;
        } return *this;}
      void SetMC(const AliHLTTPCClusterMCLabel* /*pMC*/) {assert(0);/* to be implemented*/}

      // switch to next cluster
      iterator& Next(int slice, int partition) {
        if (fData) fData->IncrementClusterCount(slice, partition);
        fSlice=slice; fPartition=partition; return operator++();
      }
      // prefix operators
      iterator& operator++() {fClusterNo++; fClusterId=GetClusterId();return *this;}
      iterator& operator--() {fClusterNo--; fClusterId=GetClusterId();return *this;}
      // postfix operators
      iterator operator++(int) {iterator i(*this); operator++(); return i;}
      iterator operator--(int) {iterator i(*this); operator--(); return i;}

      bool operator==(const iterator other) const {return fData==other.fData;}
      bool operator!=(const iterator other) const {return fData!=other.fData;}

    private:
      // retrieve cluster id from the optional cluster ID block, or calculate
      // from slice, partition and cluster number
      AliHLTUInt32_t GetClusterId() const {
        AliHLTUInt32_t id=kAliHLTVoidDataSpec;
        if (fData) id=fData->GetClusterId(fClusterNo);
        if (id==kAliHLTVoidDataSpec) id=AliHLTTPCSpacePointData::GetID(fSlice, fPartition, fClusterNo);
        return id;
      };

      int fClusterNo; //! cluster no in the current block
      AliClusterWriter* fData; //! pointer to actual data
      int fSlice;     //! current slice
      int fPartition; //! current partition
      AliHLTUInt32_t fClusterId; //! id of the cluster, from optional cluster id blocks or calculated from slice-partition-number
    };

    /// iterator of partition clusters block of specification
    iterator BeginPartitionClusterBlock(int count, AliHLTUInt32_t specification);
    /// iterator of track model clusters
    iterator BeginTrackModelClusterBlock(int count);

    /// add cluster id block for partition or track model clusters
    int AddClusterIds(const AliHLTComponentBlockData* pDesc);
    /// get the cluster id from the current cluster id block (optional)
    AliHLTUInt32_t GetClusterId(int clusterNo) const;

    /// set output buffer and init for writing
    void Init(AliHLTUInt8_t* pBuffer, AliHLTUInt32_t size);

    int Finish(AliHLTComponentBlockDataList& outputBlocks);

    /// internal cleanup
    virtual void  Clear(Option_t * option="");

    unsigned GetRequiredSpace() const {return fRequiredSpace;}

  protected:
    AliHLTComponentBlockData ReservePartitionClusterBlock(int count, AliHLTUInt32_t specification);
    AliHLTTPCRawCluster& GetClusterRef(AliHLTUInt32_t clusterId);
    int IncrementClusterCount(int slice, int partition);

  private:
    AliClusterWriter(const AliClusterWriter&);
    AliClusterWriter& operator=(const AliClusterWriter&);

    AliHLTUInt8_t* fOutputBuffer; //! output buffer for unpacked clusters
    AliHLTUInt32_t fBufferSize; //! size of output buffer
    AliHLTUInt32_t fBufferFilled; //! buffer fill count
    AliHLTUInt32_t fRequiredSpace; //! required space
    std::map<AliHLTUInt32_t, AliHLTTPCRawCluster> fTrackModelClusters; //! unpacked track model clusters
    std::map<AliHLTUInt32_t, unsigned> fTrackModelClusterCounts; //! cluster count per partition
    std::map<AliHLTUInt32_t, AliHLTComponentBlockData> fPartitionBlockDescriptors; //! block descriptors of partition clusters
    std::map<AliHLTUInt32_t, AliHLTTPCRawCluster*> fPartitionClusterTargets; //! positions of partition cluster blocks
    AliHLTTPCRawCluster* fCurrentClusterTarget; //! cluster target currently active in the iteration
    std::map<AliHLTUInt32_t, AliClusterIdBlock> fPartitionClusterIds; //! clusters ids for partition cluster ids
    AliClusterIdBlock fTrackModelClusterIds; //! cluster ids for track model clusters
    AliClusterIdBlock* fCurrentClusterIds; //! id block currently active in the iteration
    iterator fBegin; //! for returning a reference
  };

protected:
  /// inherited from AliHLTProcessor: data processing
  int DoEvent( const AliHLTComponentEventData& evtData,
               const AliHLTComponentBlockData* blocks,
               AliHLTComponentTriggerData& trigData,
               AliHLTUInt8_t* outputPtr,
               AliHLTUInt32_t& size,
               AliHLTComponentBlockDataList& outputBlocks );
  using AliHLTProcessor::DoEvent;
  int DoInit(int argc, const char** argv );

  /// inherited from AliHLTComponent: component cleanup
  int DoDeinit();

  /// inherited from AliHLTComponent: argument scan
  int ScanConfigurationArgument(int argc, const char** argv);

private:
  AliHLTTPCDataCompressionUnpackerComponent(const AliHLTTPCDataCompressionUnpackerComponent&);
  AliHLTTPCDataCompressionUnpackerComponent& operator=(const AliHLTTPCDataCompressionUnpackerComponent&);

  /// cluster decoder instance
  AliHLTTPCDataCompressionDecoder* fpDecoder; //! cluster decoder instance

  AliClusterWriter* fClusterWriter; //! writer instance to the output buffer

  unsigned fRequiredSpace; //! required space in the output buffer
  float fInputMultiplier; //! input multiplier for estimation of output size

  ClassDef(AliHLTTPCDataCompressionUnpackerComponent, 0)
};

#endif //ALIHLTTPCDATACOMPRESSIONUNPACKERCOMPONENT_H
