
namespace darc
{
namespace pubsub
{

struct subscribed_topic_record
{
  ID topic_id;

  bool operator<(const subscribed_topic_record& other) const
  {
    return topic_id < other.topic_id;
  }

  subscribed_topic_record(const ID& topic_id) :
    topic_id(topic_id)
  {
  }
};

}
}
