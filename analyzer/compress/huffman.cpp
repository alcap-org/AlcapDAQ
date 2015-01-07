//////////////////////////////////////////////////////////////////////
// 
// Huffman coding.
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "midas.h"

#include "mucap_compress.h"


typedef struct huffman_node {
  // tree left/right pointers
  struct huffman_node *branch0;
  struct huffman_node *branch1;

  // linked list pointer
  struct huffman_node *next;

  int symbol;
  int frequency;
} huffman_node;

void
huffman_put_symbol(huffman_table * table, io_buffer * buffer, int symbol)
{
  // Huffman encode a single symbol and send it to an output buffer.
  // This function is in an inner loop and therefore
  // performance-critical.

  // Update the frequency count for this symbol.
  table->frequency[symbol]++;

  // Get the code corresponding to this symbol.
  unsigned int code_bits = table->code_bits[symbol];
  int code_length = table->code_length[symbol];

  // Put it in the buffer
  io_buffer_put(buffer, code_bits, code_length);

  buffer->num_codes++;
}

#if 0
int huffman_get_symbol(huffman_table * table, io_buffer * buffer)
{
  // Get the next symbol from a Huffman-coded buffer.  Again, this 
  // function is in an inner loop and therefore performance-critical.
  // However, the current bit-by-bit implementation is for testing only 
  // and is likely to be extremely slow.

  if (buffer->num_codes <= 0) {
    return -1;
  }
  buffer->num_codes--;

  huffman_node *node = table->tree;

  while (1) {
    // We may already be there...
    if (node->symbol != -1) {
      return node->symbol;
    } else {
      // If not, consume a bit from the input and follow the tree
      if (buffer->w_bits == 0) {
	buffer->w = *((buffer->p)++);
	buffer->w_bits = 32;
      }

      unsigned int bit = buffer->w & (1u << 31);
      buffer->w <<= 1;
      buffer->w_bits--;

      if (bit == 0) {
	node = node->branch0;
      } else {
	node = node->branch1;
      }
    }
  }
}
#endif

int huffman_get_symbol(huffman_table * table, io_buffer * buffer)
{
  // Get the next symbol from a Huffman-coded buffer.  Again, this 
  // function is in an inner loop and therefore performance-critical.

  if (buffer->num_codes <= 0) {
    return -1;
  }
  buffer->num_codes--;

  // If we have at least 8 bits pre-read from the buffer, then check the
  // table of pre-computed short symbols.
  if(buffer->w_bits >= 8) {
    unsigned int byte = (buffer->w >> 24) & 0xff;
    int symbol = table->decode_symbols[byte];
    if(symbol != -1) {
      int code_length = table->code_length[symbol];
      buffer->w <<= code_length;
      buffer->w_bits -= code_length;
      return symbol; 
    } 
  }

  // Otherwise, do it the slow way, walking bit-by-bit through the tree
  huffman_node *node = table->tree;

  while (1) {
    // We may already be there...
    if (node->symbol != -1) {
      return node->symbol;
    } else {
      // If not, consume a bit from the input and follow the tree
      if (buffer->w_bits == 0) {
	buffer->w = *((buffer->p)++);
	buffer->w_bits = 32;
      }

      unsigned int bit = buffer->w & (1u << 31);
      buffer->w <<= 1;
      buffer->w_bits--;

      if (bit == 0) {
	node = node->branch0;
      } else {
	node = node->branch1;
      }
    }
  }
}

void huffman_precompute_decode(huffman_table * table)
{
  table->decode_symbols = new int[256];

  for(int i = 0; i < 256; i++) {
    int w = i << 24;
    int w_bits = 8;

    huffman_node *node = table->tree;

    while (1) {
      // We may already be there...
      if (node == NULL) {
        table->decode_symbols[i] = -1;
        break;
      } else if (node->symbol != -1) {
        table->decode_symbols[i] = node->symbol;
        break;
      } else { 
        // If not, consume a bit from the input and follow the tree
        if (w_bits == 0) {
          table->decode_symbols[i] = -1;
          break;
        }
    
        unsigned int bit = w & (1u << 31);
        w <<= 1;
        w_bits--;
  
        if (bit == 0) {
          node = node->branch0;
        } else {
          node = node->branch1;
        }
      }
    } 
  }
}

int huffman_node_compare(const void *p1, const void *p2)
{
  huffman_node **node1 = (huffman_node **) p1;
  huffman_node **node2 = (huffman_node **) p2;

  return (*node1)->frequency - (*node2)->frequency;
}

void
huffman_visit(huffman_table * table, huffman_node * node,
	      int length, unsigned int bits)
{
  int symbol = node->symbol;

  if (symbol == -1) {
    huffman_visit(table, node->branch0, length + 1, bits << 1);
    huffman_visit(table, node->branch1, length + 1, (bits << 1) | 1);
  } else {
    table->code_bits[symbol] = bits;
    table->code_length[symbol] = length;
  }
}


BOOL huffman_optimize_tree(huffman_table * table)
{
  // Start by allocating terminal nodes for the symbols.
  huffman_node *terminals[table->num_symbols];
  for (int i = 0; i < table->num_symbols; i++) {
    huffman_node *node = new huffman_node;
    node->branch0 = NULL;
    node->branch1 = NULL;
    node->next = NULL;
    node->symbol = i;
    node->frequency = table->frequency[i];
    terminals[i] = node;
  }

  // Now sort the array of terminals by frequency.
  qsort(terminals, table->num_symbols,
	sizeof(huffman_node *), huffman_node_compare);

  // Set up the linked list pointers
  for (int i = 0; i < table->num_symbols - 1; i++) {
    terminals[i]->next = terminals[i + 1];
  }
  huffman_node *root = terminals[0];

  // Now repeat: draw the two least probable nodes 
  // and combine them, sorting the resulting combination
  // into the correct position in the list.
  while (root->next != NULL) {
    huffman_node *node0 = root;
    huffman_node *node1 = node0->next;
    root = node1->next;

    huffman_node *combined = new huffman_node;
    combined->branch0 = node0;
    combined->branch1 = node1;
    combined->next = NULL;
    combined->symbol = -1;
    combined->frequency = node0->frequency + node1->frequency;

    if (root == NULL) {
      root = combined;
    } else {
      // Search through the list to find the right position for the
      // new 
      // node, and put it there.
      huffman_node *pos = root;
      huffman_node *prev = NULL;
      while (pos != NULL) {
	if (combined->frequency <= pos->frequency) {
	  combined->next = pos;
	  if (prev != NULL) {
	    prev->next = combined;
	  } else {
	    root = combined;
	  }
	  break;
	}
	prev = pos;
	pos = pos->next;
      }

      if (pos == NULL) {
	combined->next = NULL;
	prev->next = combined;
      }

#ifdef DEBUG
      // Check that we didn't screw up
      pos = root;
      prev = NULL;
      int node_count = 0;
      while (pos != NULL) {
	if (prev != NULL && pos->frequency < prev->frequency) {
          cm_msg(MERROR, "huffman_optimize_tree", "List out of order");
          return !SUCCESS;
	}
	prev = pos;
	pos = pos->next;
	node_count++;
      }
#endif
    }
  }

  table->tree = root;

  // Now we have the tree; read off the codes by recursively visiting 
  // each node.
  huffman_visit(table, root, 0, 0);

  // Check that the number of bits used by each key is less than 32.
  for (int i = 0; i < table->num_symbols; i++) {
    if(table->code_length[i] > 32) {
      cm_msg(MERROR, "huffman_optimize_tree", 
        "Code length for symbol %d is too long (%d bits)", i, 
        table->code_length[i]);
      return !SUCCESS;
    }
  }

  huffman_precompute_decode(table);

  return SUCCESS;
}

void huffman_init_default(huffman_table * table, int num_symbols)
{
  table->num_symbols = num_symbols;
  table->frequency = new int[num_symbols];
  table->code_bits = new unsigned int[num_symbols];
  table->code_length = new int[num_symbols];
  table->tree = NULL;

  // Assume that all symbols have equal probability 
  for (int i = 0; i < num_symbols; i++) {
    table->frequency[i] = 1;
  }

  huffman_optimize_tree(table);
}

BOOL save_huffman(char *key_dir, huffman_table * table)
{
  char key_name[MAX_ODB_PATH];
  int size;

  sprintf(key_name, "%s/code_bits", key_dir);
  size = table->num_symbols * sizeof(DWORD);
  int stat = db_set_value(hDB, 0, key_name, table->code_bits, size, 
    table->num_symbols, TID_DWORD); 
  if(stat != DB_SUCCESS) {
    cm_msg(MERROR, "save_huffman", "Failed to save ODB key %s", key_name);
    return !SUCCESS;
  }

  sprintf(key_name, "%s/code_length", key_dir);
  stat = db_set_value(hDB, 0, key_name, table->code_length, size, 
    table->num_symbols, TID_DWORD); 
  if(stat != DB_SUCCESS) {
    cm_msg(MERROR, "save_huffman", "Failed to save ODB key %s", key_name);
    return !SUCCESS;
  }

  sprintf(key_name, "%s/frequency", key_dir);
  stat = db_set_value(hDB, 0, key_name, table->frequency, size, 
    table->num_symbols, TID_DWORD); 
  if(stat != DB_SUCCESS) {
    cm_msg(MERROR, "save_huffman", "Failed to save ODB key %s", key_name);
    return !SUCCESS;
  }

  return SUCCESS;
}

void init_huffman_node(huffman_node *node)
{
  node->branch0 = NULL;
  node->branch1 = NULL;
  node->next = NULL;
  node->symbol = -1;
  node->frequency = 0;
}

void huffman_delete_tree(huffman_node *node)
{
  if(node->branch0 != NULL) {
    huffman_delete_tree(node->branch0);
  }
 
  if(node->branch1 != NULL) {
    huffman_delete_tree(node->branch1);
  } 

  delete node;
}

BOOL huffman_build_tree(huffman_table * table) 
{
  // build the Huffman tree corresponding to the bit strings in code_bits 
  // and code_length

  // initialize root of tree
  huffman_node *root = new huffman_node;
  table->tree = root;
  init_huffman_node(root);

  for(int i = 0; i < table->num_symbols; i++) {
    DWORD code_bits = table->code_bits[i];
    int code_length = table->code_length[i];
    code_bits <<= 32-code_length;

    huffman_node *node = root;

    for(int bit_number = 0; bit_number < code_length; bit_number++) {

      BOOL bit = ((code_bits & (1u << 31)) != 0); 
      code_bits <<= 1;

      if(bit == 0) {
        if(node->branch0 == NULL) {
          node->branch0 = new huffman_node;
          init_huffman_node(node->branch0);
        }
        node = node->branch0;

        if(bit_number == code_length - 1) {
          node->symbol = i;
          node->frequency = table->frequency[i];
        }
      } else {
        if(node->branch1 == NULL) {
          node->branch1 = new huffman_node;
          init_huffman_node(node->branch1);
        }
        node = node->branch1;

        if(bit_number == code_length - 1) {
          node->symbol = i;
          node->frequency = table->frequency[i];
        }
      }
    }
  }

  huffman_precompute_decode(table);

  return SUCCESS;
}

BOOL load_huffman(char *key_dir, huffman_table * table)
{
  char key_name[MAX_ODB_PATH];
  int size;

  for(int i = 0; i < table->num_symbols; i++) {
    table->frequency[i] = 1;
  }

  sprintf(key_name, "%s/code_bits", key_dir);
  size = table->num_symbols * sizeof(DWORD);
  int stat = db_get_value(hDB, 0, key_name, table->code_bits, &size, 
    TID_DWORD, FALSE); 
  if(stat != DB_SUCCESS || size != table->num_symbols * sizeof(DWORD)) {
    cm_msg(MERROR, "load_huffman", "Failed to load ODB key %s", key_name);
    return !SUCCESS;
  }

  sprintf(key_name, "%s/code_length", key_dir);
  size = table->num_symbols * sizeof(DWORD);
  db_get_value(hDB, 0, key_name, table->code_length, &size, TID_DWORD, FALSE); 
  if(stat != DB_SUCCESS || size != table->num_symbols * sizeof(DWORD)) {
    cm_msg(MERROR, "load_huffman", "Failed to load ODB key %s", key_name);
    return !SUCCESS;
  }

  huffman_build_tree(table);

  return SUCCESS;
}
