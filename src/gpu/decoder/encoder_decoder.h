#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

#include "common/scorer.h"
#include "common/loader.h"
#include "common/base_best_hyps.h"
#include "common/threadpool.h"
#include "gpu/types-gpu.h"


namespace GPU {

class EncoderDecoderState;
class Encoder;
class Decoder;
class Weights;

namespace mblas {
  template <class VecType>
  class TMatrix;
  typedef TMatrix<DeviceVector<float>> Matrix;
}

////////////////////////////////////////////
class EncoderDecoder : public Scorer {
  private:
    typedef EncoderDecoderState EDState;

  public:
    EncoderDecoder(God &god,
    			   const std::string& name,
                   const YAML::Node& config,
                   size_t tab,
                   const Weights& model);

    virtual void Score(God &god, const State& in, State& out, const std::vector<size_t>& beamSizes);

    virtual State* NewState();

    virtual void BeginSentenceState(State& state, size_t batchSize=1);

    virtual void SetSource(const Sentences& source);

    virtual void AssembleBeamState(const State& in,
                                   const Beam& beam,
                                   State& out);

    void GetAttention(mblas::Matrix& Attention);

    mblas::Matrix& GetAttention();
    virtual BaseMatrix& GetProbs();

    size_t GetVocabSize() const;

    void Filter(const std::vector<size_t>& filterIds);

    virtual ~EncoderDecoder();

  private:
    const Weights& model_;
    std::unique_ptr<Encoder> encoder_;
    std::unique_ptr<Decoder> decoder_;
    DeviceVector<size_t> indices_;
    DeviceVector<int> batchMapping_;

    std::unique_ptr<mblas::Matrix> SourceContext_;

    EncoderDecoder(const EncoderDecoder&) = delete;
};

////////////////////////////////////////////
class EncoderDecoderLoader : public Loader {
  public:
	EncoderDecoderLoader(const EncoderDecoderLoader&) = delete;
    EncoderDecoderLoader(const std::string name,
                         const YAML::Node& config);

    virtual void Load(God &god);

    virtual ScorerPtr NewScorer(God &god, size_t taskId);
    virtual BestHypsBase &GetBestHyps(God &god);

  private:
    std::vector<std::unique_ptr<Weights>> weights_;
};

}
