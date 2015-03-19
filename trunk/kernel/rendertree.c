#include <algorithm.h>
#include <memory.h>
#include <logout.h>
#include <x3d/rendertree.h>


static void render_node_init(struct render_node* node,
                             bool (*free_self)       (struct render_node* self),
                             bool (*verify_self)     (struct render_tree* tree, struct render_node* self),
                             bool (*insert_input)    (struct render_node* self, struct render_node* input),
                             bool (*remove_input)    (struct render_node* self, struct render_node* input),
                             char* name, int n_input, int n_output);
static void render_node_free(struct render_node* node);
static void render_node_set_input(struct render_node* node, struct render_node* input, int i);
static void render_node_remove_input(struct render_node* node, struct render_node* input);
static enum RENDER_NODE_IDR render_node_get_type(struct render_node* node);
static const char* render_node_get_name(struct render_node* node);
static void render_node_copy_link(struct render_node* src, struct render_node* dest);

static bool render_tree_check_environment(struct render_tree* tree, enum RENDER_ENVIRONMENT type, char* name);

/*
 * render_output
 */
static bool render_node_output_free(struct render_node* self)
{
        render_node_free(self);
        struct render_output* node = (struct render_output*) self;
        free_fix(node->probe);
        free_fix(node);
        return true;
}

static bool render_node_output_insert(struct render_node* self, struct render_node* input)
{
        render_node_set_input(self, input, 0);
        return true;
}

static bool render_node_output_remove(struct render_node* self, struct render_node* input)
{
        render_node_remove_input(self, input);
        return true;
}

static bool render_node_verify_self(struct render_tree* tree, struct render_node* self)
{
        return render_tree_check_environment(tree, RENDER_ENV_PROBE, ((struct render_output*) self)->probe);
}

struct render_node* render_node_output_create(char* name, char* probe)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_output* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_output_free,
                         render_node_verify_self,
                         render_node_output_insert,
                         render_node_output_remove,
                         name, c_NumInput, c_NumOutput);
        node->probe = "";
        return (struct render_node*) node;
}

/*
 * render_layer
 */
struct render_node* render_node_layer_create(char* name)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_layer* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         name, c_NumInput, c_NumOutput);
        return (struct render_node*) node;
}

/*
 * render_rdacontext
 */
static bool render_node_rdacontext_free(struct render_node* self)
{
        render_node_free(self);
        struct render_rdacontext* node = (struct render_rdacontext*) self;
        free_fix(node->rdacontext);
        free_fix(node);
        return true;
}

static bool render_node_rdacontext_insert(struct render_node* self, struct render_node* input)
{
        log_mild_err_dbg("cannot insert input to renderable context node");
        return false;
}

static bool render_node_rdacontext_remove(struct render_node* self, struct render_node* input)
{
        log_mild_err_dbg("cannot remove input from renderable context node, nothing to be removed");
        return false;
}

static bool render_node_rdacontext_self(struct render_tree* tree, struct render_node* self)
{
        return render_tree_check_environment(tree, RENDER_ENV_RDA, ((struct render_rdacontext*) self)->rdacontext);
}

struct render_node* render_node_rdacontext_create(char* name, char* rdacontext, enum RENDER_ACC_IDR access)
{
        static const int c_NumInput = 0;
        static const int c_NumOutput = 1;

        struct render_rdacontext* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_rdacontext_free,
                         render_node_rdacontext_self,
                         render_node_rdacontext_insert,
                         render_node_rdacontext_remove,
                         name, c_NumInput, c_NumOutput);
        node->rdacontext = alg_alloc_string(rdacontext);
        return (struct render_node*) node;
}

/*
 * render_radiance
 */
static bool render_node_radiance_free(struct render_node* self)
{
        render_node_free(self);
        free_fix(self);
        return true;
}

static bool render_node_radiance_insert(struct render_node* self, struct render_node* input)
{
        static const int c_RenderableContextSlot = 0;

        enum RENDER_NODE_IDR type = render_node_get_type(input);
        switch(type) {
        case RENDER_NODE_RDACONTEXT:
                {
                render_node_set_input(self, input, c_RenderableContextSlot);
                return true;
                }
        default:
                {
                log_mild_err_dbg("unsupported input type: %d", type);
                return false;
                }
        }
}

static bool render_node_radiance_remove(struct render_node* self, struct render_node* input)
{
        render_node_remove_input(self, input);
        return true;
}

static bool render_node_radiance_self(struct render_tree* tree, struct render_node* self)
{
        return true;
}

struct render_node* render_node_radiance_create(char* name, enum RENDER_PIPE_IDR pipe)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_radiance* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_radiance_free,
                         render_node_radiance_self,
                         render_node_radiance_insert,
                         render_node_radiance_remove,
                         name, c_NumInput, c_NumOutput);
        node->pipe = pipe;
        return (struct render_node*) node;
}

/*
 * class render_tree
 */
static void render_node_init(struct render_node* node,
                             bool (*free_self)       (struct render_node* self),
                             bool (*verify_self)     (struct render_tree* tree, struct render_node* self),
                             bool (*insert_input)    (struct render_node* self, struct render_node* input),
                             bool (*remove_input)    (struct render_node* self, struct render_node* input),
                             char* name, int n_input, int n_output)
{
        zero_obj(node);
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

static void render_node_free(struct render_node* node)
{
        free_fix(node->name);
        free_fix(node->input);
        free_fix(node->output);
        zero_obj(node);
}

static void render_node_set_input(struct render_node* node, struct render_node* input, int i)
{
        node->input[i] = input;
        input->output[input->i_output ++] = node;
}

static void render_node_remove_input(struct render_node* node, struct render_node* input)
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

static enum RENDER_NODE_IDR render_node_get_type(struct render_node* node)
{
        return node->type;
}

static const char* render_node_get_name(struct render_node* node)
{
        return node->name;
}

static void render_node_copy_link(struct render_node* src, struct render_node* dest)
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

static bool render_tree_check_environment(struct render_tree* tree, enum RENDER_ENVIRONMENT type, char* name)
{
        if (type != RENDER_ENV_VOID) {
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
        int i;
        for (i = 0; i < sizeof(tree->var)/sizeof(tree->var[0]); i ++) {
                int j;
                for (j = 0; j < tree->n_var[i]; j ++) {
                        free_fix(tree->var[i][j].name);
                        tree->var[i][j].ptr = nullptr;
                }
        }
        zero_obj(tree);
}

struct render_node* render_tree_create_root(struct render_tree* tree)
{
        struct render_node* root = alloc_obj(root);
        render_node_init(root,
                         nullptr, nullptr, nullptr, nullptr,
                         "root", 1, 0);
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

struct render_node* render_tree_get_node(struct render_tree* tree, char* name)
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
        int i;
        for (i = 0; i < node->n_input; i ++) {
                render_node_remove_input(node, node->input[i]);
        }
        node->ops.free_self(node);
        return true;
}

void render_tree_declare_environment(struct render_tree* tree,
                                     enum RENDER_ENVIRONMENT type,
                                     char* var_name, void* var)
{
        int spot = tree->n_var[type] ++;
        tree->var[type][spot].name = alg_alloc_string(var_name);
        tree->var[type][spot].ptr = var;
}

struct node_info {
        const char*     name;
        int             out_reg;
};

struct visit_compile_data {
        declare_stack(stack, 128*sizeof(struct render_node*));
        char*                           bytecode;
        d_alg_list(struct node_info)    node_infos;
        int                             reg_count;
        bool                            is_succeeded;
};

static void visit_compile_push(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct visit_compile_data* data = dataptr;
        if (!data->is_succeeded)
                return ;
        push_stack(&data->stack, node);
}

#define write_bytecode(_dest, _code) \
{ \
        *(typeof(_code)*)(_dest) = (_code); \
        _dest += sizeof(_code); \
}

static void visit_compile_backtrack(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct visit_compile_data* data = dataptr;
        if (!data->is_succeeded)
                return ;
        struct render_node* op_node;
        pop_stack(&data->stack, op_node);
        // generate instruction
        if (op_node->n_output != 0) {
#define node_info_cmp(_data, _node_info)          (!strcmp(_data, (_node_info)->name))
                alg_iter(struct node_info) iter;
                alg_find(list, &data->node_infos, render_node_get_name(op_node), iter, node_info_cmp);
                struct node_info info;
                if (iter == nullptr) {
                        // new operator, allocate register
                        info.name = render_node_get_name(op_node);
                        info.out_reg = data->reg_count ++;
                        alg_push_back(list, &data->node_infos, info);
                } else {
                        info = alg_access(iter);
                }
                int op_size = sizeof(op_size) + 4 + sizeof(int) + op_node->n_input*sizeof(int);
                write_bytecode(data->bytecode, op_size);                        printf("%d ", op_size);
                write_bytecode(data->bytecode, render_node_get_type(node));     printf("%d ", render_node_get_type(node));
                write_bytecode(data->bytecode, info.out_reg);                   printf("%d ", info.out_reg);
                int i;
                for (i = 0; i < op_node->n_input; i ++) {
                        alg_find(list, &data->node_infos, render_node_get_name(op_node->input[i]), iter, node_info_cmp);
                        if (iter == nullptr) {
                                log_severe_err("input: %s of operator node: %s doesn't have an allocated register",
                                                render_node_get_name(op_node), render_node_get_name(op_node->input[i]));
                                log_severe_err("generation halt");
                                data->is_succeeded = false;
                        }
                        write_bytecode(data->bytecode, alg_access(iter).out_reg);printf("%d ", alg_access(iter).out_reg);
                }
                printf("\n");
#undef node_info_cmp
        } else {
                log_normal_dbg("operator node: %s doesn't have output", render_node_get_name(op_node));
        }
}

bool render_tree_compile(struct render_tree* tree, struct render_bytecode* bytecode)
{
        struct visit_compile_data data;
        data.bytecode           = bytecode->instr;
        data.is_succeeded       = true;
        data.reg_count          = 0;
        create_alg_list(&data.node_infos, sizeof(struct node_info), 1);

        struct render_tree_visitor visitor;
        render_tree_visitor_init2(&visitor, visit_compile_push, visit_compile_backtrack, &data);
        render_tree_visit(tree, &visitor);

        alg_free(list, &data.node_infos);
        return data.is_succeeded;
}

static bool is_empty_input(int* c_ibranch, struct render_node* node)
{
        int i;
        for (i = 0; i < node->n_input; i ++) {
                if (node->input[i] != nullptr) {
                        *c_ibranch = i;
                        return true;
                }
        }
        *c_ibranch = -1;
        return false;
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
        if (c_node == nullptr || is_empty_input(&c_ibranch, c_node)) {
                backtrack_tree_branch(&stack, r0,
                                      pop_stack(&stack, c_ibranch);
                                      pop_stack(&stack, c_node);
                                      c_node = next_input(&c_ibranch, c_node););
        } else {
                enter_tree_branch(visitor->this_node(c_node, c_node->input, c_node->output, visitor->dataptr);
                                  push_stack(&stack, c_node);
                                  push_stack(&stack, c_ibranch);
                                  c_node = c_node->input[c_ibranch];);
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
