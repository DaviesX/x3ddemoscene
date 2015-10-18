#include <misc.h>
#include <container/stack.h>
#include <container/arraylist.h>
#include <system/allocator.h>
#include <system/log.h>
#include <x3d/rendertree.h>


/*
 * class render_tree
 */
void render_node_init(struct render_node* node,
                             bool (*free_self)       (struct render_node* self),
                             bool (*verify_self)     (struct render_tree* tree, struct render_node* self),
                             bool (*insert_input)    (struct render_node* self, struct render_node* input),
                             bool (*remove_input)    (struct render_node* self, struct render_node* input),
                             enum RenderNodeType type, const char* name, int n_input, int n_output)
{
        zero_obj(node);
        node->type = type;
        node->name = alg_alloc_string(name);
        node->ops.free_self = free_self;
        node->ops.verify_self = verify_self;
        node->ops.insert_input = insert_input;
        node->ops.remove_input = remove_input;
        node->n_input = n_input;
        node->n_output = (n_output != 0) ? 20 : 0;
        node->i_output = 0;
        node->input = alloc_var(sizeof(struct render_node*), n_input);
        zero_array(node->input, n_input);
        node->output = alloc_fix(sizeof(struct render_node*), node->n_output);
        zero_array(node->output, node->n_output);
}

void render_node_free(struct render_node* node)
{
        free_fix(node->name);
        free_fix(node->input);
        free_fix(node->output);
        zero_obj(node);
}

void render_node_set_input(struct render_node* node, struct render_node* input, int i)
{
        node->input[i] = input;
        input->output[input->i_output ++] = node;
}

void render_node_remove_input(struct render_node* node, struct render_node* input)
{
        int i, j;
        for (i = 0, j = 0; i < input->i_output; i ++) {
                if (input->output[i] != node) {
                        j ++;
                }
                input->output[j] = input->output[i];
        }
        input->i_output --;
        input->input[i] = nullptr;
}

enum RenderNodeType render_node_get_type(struct render_node* node)
{
        return node->type;
}

const char* render_node_get_name(struct render_node* node)
{
        return node->name;
}

void render_node_copy_link(struct render_node* src, struct render_node* dest)
{
        dest->n_input = src->n_input;
        memcpy(dest->input, src->input, sizeof(*dest->input)*dest->n_input);

        dest->i_output = src->i_output;
        dest->n_output = src->n_output;
        memcpy(dest->output, src->output, sizeof(*dest->output)*dest->n_output);

        int i;
        for (i = 0; i < dest->n_input; i ++) {
                render_node_remove_input(src, dest->input[i]);
                render_node_set_input(dest, dest->input[i], i);
        }
        for (i = 0; i < dest->n_output; i ++) {
                if (dest->output[i] == nullptr)
                        break;
                struct render_node* output = dest->output[i];
                int j;
                for (j = 0; j < output->n_input; j ++) {
                        if (output->input[j] == src) {
                                output->input[j] = dest;
                        }
                }
        }
}

bool render_tree_check_environment(struct render_tree* tree, enum RenderEnvironment type, char* name)
{
        if (type != RenderEnvVoid) {
                int i;
                for (i = 0; i < tree->n_var[type]; i ++) {
                        if (!strcmp(name, tree->var[type][i].name) && tree->var[type][i].ptr != nullptr) {
                                return true;
                        }
                }
                return false;
        } else {
                int i, j;
                for (j = 0; j < sizeof(tree->n_var)/sizeof(int); j ++ ) {
                        for (i = 0; i < tree->n_var[j]; i ++) {
                                if (!strcmp(name, tree->var[type][i].name) && tree->var[type][i].ptr != nullptr) {
                                        return true;
                                }
                        }
                }
                return false;
        }
}

void render_tree_init(struct render_tree* tree)
{
        zero_obj(tree);
}

static void visit_free_node(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        node->ops.free_self(node);
}

void render_tree_free(struct render_tree* tree)
{
        struct render_tree_visitor visitor;
        render_tree_visitor_init(&visitor, visit_free_node, nullptr);
        render_tree_visit(tree, &visitor);
        render_tree_clear_environment(tree);
        zero_obj(tree);
}

static bool render_node_root_free(struct render_node* self)
{
        render_node_free(self);
        free_fix(self);
        return true;
}

static bool render_node_root_verify(struct render_tree* tree, struct render_node* self)
{
        return true;
}

static bool render_node_root_insert(struct render_node* self, struct render_node* input)
{
        render_node_set_input(self, input, 0);
        return true;
}

static bool render_node_root_remove(struct render_node* self, struct render_node* input)
{
        render_node_remove_input(self, input);
        return true;
}

struct render_node* render_tree_create_root(struct render_tree* tree)
{
        struct render_node* root = alloc_obj(root);
        render_node_init(root,
                         render_node_root_free,
                         render_node_root_verify,
                         render_node_root_insert,
                         render_node_root_remove,
                         RenderNodeRoot, "root", 1, 0);
        tree->node_tree = root;
        return root;
}

struct visit_get_node_data {
        const char*             name;
        struct render_node*     node_returned;
};

static void visit_get_node(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct visit_get_node_data* data = dataptr;
        if (!strcmp(render_node_get_name(node), data->name)) {
                data->node_returned = node;
        }
}

struct render_node* render_tree_get_node(struct render_tree* tree, const char* name)
{
        struct visit_get_node_data data = {name, nullptr};
        struct render_tree_visitor visitor;
        render_tree_visitor_init(&visitor, visit_get_node, &data);
        render_tree_visit(tree, &visitor);
        return data.node_returned;
}

bool render_tree_insert_node(struct render_tree* tree,
                             struct render_node* parent,
                             struct render_node* node)
{
        parent->ops.insert_input(parent, node);
        return parent->ops.verify_self(tree, parent);
}

bool render_tree_replace_node(struct render_tree* tree, struct render_node* node)
{
        if (!node->ops.verify_self(tree, node)) {
                return false;
        }
        struct render_node* origin = render_tree_get_node(tree, (char*) render_node_get_name(node));
        render_node_copy_link(origin, node);
        origin->ops.free_self(origin);
        return true;
}

bool render_tree_remove_node(struct render_tree* tree,
                             struct render_node* parent,
                             struct render_node* node)
{
        render_node_remove_input(parent, node);
        if (node->n_output == 0) {
                int i;
                for (i = 0; i < node->n_input; i ++) {
                        render_node_remove_input(node, node->input[i]);
                }
                node->ops.free_self(node);
        }
        return true;
}

void render_tree_declare_environment(struct render_tree* tree,
                                     enum RenderEnvironment type,
                                     const char* var_name, void* var)
{
        int spot = tree->n_var[type] ++;
        tree->var[type][spot].name = alg_alloc_string(var_name);
        tree->var[type][spot].ptr = var;
}

void* render_tree_retrieve_environment(struct render_tree* tree, const char* name, enum RenderEnvironment type)
{
        if (name) {
                int i;
                for (i = 0; i < tree->n_var[type]; i ++) {
                        if (!strcmp(name, tree->var[type][i].name)) {
                                return tree->var[type][i].ptr;
                        }
                }
                return nullptr;
        } else {
                return tree->var[type][tree->n_var[type] - 1].ptr;
        }
}

void render_tree_clear_environment(struct render_tree* tree)
{
        int i;
        for (i = 0; i < sizeof(tree->var)/sizeof(tree->var[0]); i ++) {
                int j;
                for (j = 0; j < tree->n_var[i]; j ++) {
                        free_fix(tree->var[i][j].name);
                        tree->var[i][j].ptr = nullptr;
                }
                tree->n_var[i] = 0;
        }
}

static bool is_empty_input(int* c_ibranch, struct render_node* node)
{
        int i;
        for (i = 0; i < node->n_input; i ++) {
                if (node->input[i] != nullptr) {
                        *c_ibranch = i;
                        return false;
                }
        }
        *c_ibranch = -1;
        return true;
}

static struct render_node* first_input(int* c_ibranch, struct render_node* node)
{
        int i;
        for (i = *c_ibranch; i < node->n_input; i ++) {
                if (node->input[i] != nullptr) {
                        *c_ibranch = i;
                        return node->input[i];
                }
        }
        return nullptr;
}

static struct render_node* next_input(int* c_ibranch, struct render_node* node)
{
        int i;
        for (i = *c_ibranch + 1; i < node->n_input; i ++) {
                if (node->input[i] != nullptr) {
                        *c_ibranch = i;
                        return node->input[i];
                }
        }
        return nullptr;
}

bool render_tree_visit(struct render_tree* tree, struct render_tree_visitor* visitor)
{
        declare_stack(stack, 128*sizeof(struct render_node*));
        init_stack(&stack);
        struct render_node*     c_node = tree->node_tree;
        int                     c_ibranch = 0;

        begin_tree_traversal(r0);
        if (c_node == nullptr/* || is_empty_input(&c_ibranch, c_node)*/) {
                backtrack_tree_branch(&stack, r0,
                                      pop_stack(&stack, c_ibranch);
                                      pop_stack(&stack, c_node);
                                      if (visitor->backtrack) {
                                              if (c_node == nullptr) {
                                                      log_critical_err_dbg("encounter nullptr node during traversal. logical error!");
                                              }
                                              visitor->backtrack(c_node, c_node->input, c_node->output, visitor->dataptr);
                                      }
                                      c_node = next_input(&c_ibranch, c_node););
        } else {
                enter_tree_branch(if (visitor->this_node) {
                                          visitor->this_node(c_node, c_node->input, c_node->output, visitor->dataptr);
                                  }
                                  push_stack(&stack, c_node);
                                  push_stack(&stack, c_ibranch);
                                  c_node = first_input(&c_ibranch, c_node););
        }
        end_tree_traversal(r0);
        return true;
}

void render_tree_visitor_init(struct render_tree_visitor* visitor,
                              void (*this_node) (struct render_node* node,
                                                 struct render_node* input[],
                                                 struct render_node* output[],
                                                 void* dataptr),
                              void* dataptr)
{
        zero_obj(visitor);
        visitor->this_node = this_node;
        visitor->dataptr = dataptr;
}

void render_tree_visitor_init2(struct render_tree_visitor* visitor,
                              void (*this_node) (struct render_node* node,
                                                 struct render_node* input[],
                                                 struct render_node* output[],
                                                 void* dataptr),
                               void (*backtrack) (struct render_node* node,
                                                  struct render_node* input[],
                                                  struct render_node* output[],
                                                  void* dataptr),
                              void* dataptr)
{
        render_tree_visitor_init(visitor, this_node, dataptr);
        visitor->backtrack = backtrack;
}


struct node_info {
        const char*     name;
        int             out_reg;
};

struct visit_verify_data {
        bool                            is_succeeded;
        struct render_tree*             tree;
};

static void visit_compile_push(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct visit_verify_data* data = dataptr;
        bool is_succeeded = node->ops.verify_self(data->tree, node);
        if (!data->is_succeeded) {
                return ;
        } else {
                data->is_succeeded = is_succeeded;
        }
}

bool render_tree_verify(struct render_tree* self)
{
        struct visit_verify_data        data;
        data.is_succeeded       = true;
        data.tree               = self;

        struct render_tree_visitor visitor;
        render_tree_visitor_init(&visitor, visit_compile_push, &data);
        render_tree_visit(self, &visitor);

        return data.is_succeeded;
}

static const char* c_NodeTypeToString[] = {
        "RenderNodeRoot",
        "RenderNodeRenderableLoader",
        "RenderNodeLayerLoader",
        "RenderNodeProbeLoader",
        "RenderNodeRadiance",
        "RenderNodeOutput",
};

static void __visit_print_push(struct render_node* node, struct render_node* input[], struct render_node* output[],
                               void* dataptr)
{
        log_normal_dbg("PUSH    : [name:%s, type: %s]",
                       render_node_get_name(node), render_node_get_type(node));
        int i;
        for (i = 0; i < node->n_input; i ++) {
                if (input[i] != nullptr) {
                        log_normal_dbg("\tINPUT    : [name:%s, type: %s]",
                                       render_node_get_name(input[i]),
                                       c_NodeTypeToString[render_node_get_type(input[i])]);
                }
        }
        for (i = 0; i < node->n_output; i ++) {
                if (output[i] != nullptr) {
                        log_normal_dbg("\tOUTPUT    : [name:%s, type: %s]",
                                       render_node_get_name(output[i]),
                                       c_NodeTypeToString[render_node_get_type(output[i])]);
                }
        }
}

static void __visit_print_backtrack(struct render_node* node, struct render_node* input[], struct render_node* output[],
                                    void* dataptr)
{
        log_normal_dbg("BACKTRACK: [name:%s, type: %s]",
                       render_node_get_name(node), render_node_get_type(node));
        int i;
        for (i = 0; i < node->n_input; i ++) {
                if (input[i] != nullptr) {
                        log_normal_dbg("\tINPUT    : [name:%s, type: %s]",
                                       render_node_get_name(input[i]),
                                       c_NodeTypeToString[render_node_get_type(input[i])]);
                }
        }
        for (i = 0; i < node->n_output; i ++) {
                if (output[i] != nullptr) {
                        log_normal_dbg("\tOUTPUT    : [name:%s, type: %s]",
                                       render_node_get_name(output[i]),
                                       c_NodeTypeToString[render_node_get_type(output[i])]);
                }
        }
}

void render_tree_print(struct render_tree* self)
{
        log_normal_dbg("Print result of tree: %x", self);
        struct render_tree_visitor      visitor;
        render_tree_visitor_init2(&visitor, __visit_print_push, __visit_print_backtrack, nullptr);
}
