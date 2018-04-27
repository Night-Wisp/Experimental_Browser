#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml++/libxml++.h>
 
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
 
#include <iostream>
#include <string>
 
#define HEADER_ACCEPT "Accept:text/html,application/xhtml+xml,application/xml"
#define HEADER_USER_AGENT "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.70 Safari/537.17"

struct node
{
  int key_value;
  node *left;
  node *right;
};

class btree
{
    public:
        btree();
        ~btree();

        void insert(int key);
        node *search(int key);
        void destroy_tree();

    private:
        void destroy_tree(node *leaf);
        void insert(int key, node *leaf);
        node *search(int key, node *leaf);
        
        node *root;
};

btree::btree()
{
  root=NULL;
}
void btree::destroy_tree(node *leaf)
{
  if(leaf!=NULL)
  {
    destroy_tree(leaf->left);
    destroy_tree(leaf->right);
    delete leaf;
  }
}
void btree::insert(int key, node *leaf)
{
  if(key< leaf->key_value)
  {
    if(leaf->left!=NULL)
     insert(key, leaf->left);
    else
    {
      leaf->left=new node;
      leaf->left->key_value=key;
      leaf->left->left=NULL;    //Sets the left child of the child node to null
      leaf->left->right=NULL;   //Sets the right child of the child node to null
    }  
  }
  else if(key>=leaf->key_value)
  {
    if(leaf->right!=NULL)
      insert(key, leaf->right);
    else
    {
      leaf->right=new node;
      leaf->right->key_value=key;
      leaf->right->left=NULL;  //Sets the left child of the child node to null
      leaf->right->right=NULL; //Sets the right child of the child node to null
    }
  }
}
node *btree::search(int key, node *leaf)
{
  if(leaf!=NULL)
  {
    if(key==leaf->key_value)
      return leaf;
    if(key<leaf->key_value)
      return search(key, leaf->left);
    else
      return search(key, leaf->right);
  }
  else return NULL;
}
void btree::insert(int key)
{
  if(root!=NULL)
    insert(key, root);
  else
  {
    root=new node;
    root->key_value=key;
    root->left=NULL;
    root->right=NULL;
  }
}
node *btree::search(int key)
{
  return search(key, root);
}
void btree::destroy_tree()
{
  destroy_tree(root);
}

int main() {

    std::string url = "http://www.iplocation.net/";
    curlpp::Easy request;
 
    // Specify the URL
    request.setOpt(curlpp::options::Url(url));
 
    // Specify some headers
    std::list<std::string> headers;
    headers.push_back(HEADER_ACCEPT);
    headers.push_back(HEADER_USER_AGENT);
    request.setOpt(new curlpp::options::HttpHeader(headers));
    request.setOpt(new curlpp::options::FollowLocation(true));
 
    // Configure curlpp to use stream
    std::ostringstream responseStream;
    curlpp::options::WriteStream streamWriter(&responseStream);
    request.setOpt(streamWriter);
 
    // Collect response
    request.perform();
    std::string re = responseStream.str();
 
    // Parse HTML and create a DOM tree
    xmlDoc* doc = htmlReadDoc((xmlChar*)re.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
 
    // Encapsulate raw libxml document in a libxml++ wrapper
    xmlNode* r = xmlDocGetRootElement(doc);
    xmlpp::Element* root = new xmlpp::Element(r);
 
    // Grab the IP address
    std::string xpath = "//*[@id=\"locator\"]/p[1]/b/font/text()";
    auto elements = root->find(xpath);
    std::cout << "Your IP address is:" << std::endl;
    std::cout << dynamic_cast<xmlpp::ContentNode*>(elements[0])->get_content() << std::endl;
 
    delete root;
    xmlFreeDoc(doc);
 
    return 0;

}
